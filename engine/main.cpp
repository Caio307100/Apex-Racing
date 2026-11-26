#include <SDL2/SDL.h>
#include <GL/gl.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>

namespace apex {
constexpr float PI = 3.14159265358979323846f;
constexpr float TRACK_X = 48.0f;
constexpr float TRACK_Z = 28.0f;
constexpr float ROAD_HALF_WIDTH = 7.0f;

struct Vec3 {
    float x{}, y{}, z{};
    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
};

float dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}
Vec3 normalized(const Vec3& v) {
    const float length = std::sqrt(std::max(0.00001f, dot(v, v)));
    return v * (1.0f / length);
}

struct Vehicle {
    Vec3 position{TRACK_X, 0.55f, 0.0f};
    float yaw{PI};
    float speed{};
    float steering{};
    int laps{};
    float previousTrackAngle{0.0f};

    void reset() { *this = Vehicle{}; }

    void update(float dt, const Uint8* keys) {
        const bool throttle = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
        const bool brake = keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN];
        const bool left = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
        const bool right = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];

        if (throttle) speed += 25.0f * dt;
        if (brake) speed -= 38.0f * dt;
        if (!throttle && !brake) speed *= std::pow(0.985f, dt * 60.0f);
        speed = std::clamp(speed, -12.0f, 62.0f);

        const float steerInput = (left ? 1.0f : 0.0f) - (right ? 1.0f : 0.0f);
        steering += (steerInput - steering) * std::min(1.0f, dt * 7.0f);
        const float speedFactor = std::clamp(std::abs(speed) / 18.0f, 0.0f, 1.0f);
        yaw -= steering * speedFactor * (speed >= 0.0f ? 1.0f : -1.0f) * 1.75f * dt;

        const Vec3 forward{std::sin(yaw), 0.0f, -std::cos(yaw)};
        position = position + forward * (speed * dt);

        const float normalizedRadius = std::sqrt(
            (position.x * position.x) / (TRACK_X * TRACK_X) +
            (position.z * position.z) / (TRACK_Z * TRACK_Z));
        const float distanceFromCenter = std::abs(normalizedRadius - 1.0f) * 32.0f;
        if (distanceFromCenter > ROAD_HALF_WIDTH - 1.0f) {
            speed *= std::pow(0.995f, dt * 60.0f);
        }

        float angle = std::atan2(position.z / TRACK_Z, position.x / TRACK_X);
        if (angle < 0.0f) angle += 2.0f * PI;
        if (previousTrackAngle > 5.8f && angle < 0.45f && speed > 2.0f) ++laps;
        previousTrackAngle = angle;
        position.y = 0.55f;
    }
};

class Engine {
public:
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            std::fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
            return false;
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        window = SDL_CreateWindow("Apex Racing Engine 0.2.0 | C++",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        if (!window) {
            std::fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
            return false;
        }
        context = SDL_GL_CreateContext(window);
        if (!context) {
            std::fprintf(stderr, "SDL_GL_CreateContext: %s\n", SDL_GetError());
            return false;
        }
        SDL_GL_SetSwapInterval(1);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_NORMALIZE);
        glShadeModel(GL_SMOOTH);
        glClearColor(0.48f, 0.70f, 0.91f, 1.0f);
        const GLfloat globalAmbient[]{0.34f, 0.36f, 0.40f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        running = true;
        return true;
    }

    void run() {
        Uint64 previous = SDL_GetPerformanceCounter();
        while (running) {
            const Uint64 now = SDL_GetPerformanceCounter();
            float dt = static_cast<float>(now - previous) /
                       static_cast<float>(SDL_GetPerformanceFrequency());
            previous = now;
            dt = std::min(dt, 0.05f);

            processEvents();
            const Uint8* keys = SDL_GetKeyboardState(nullptr);
            if (!paused) vehicle.update(dt, keys);
            render();
            updateTitle();
        }
    }

    ~Engine() {
        if (context) SDL_GL_DeleteContext(context);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }

private:
    SDL_Window* window{};
    SDL_GLContext context{};
    int width{1280}, height{720};
    bool running{}, paused{};
    Vehicle vehicle;
    float elapsed{};

    void processEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                width = std::max(1, event.window.data1);
                height = std::max(1, event.window.data2);
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (event.key.keysym.sym == SDLK_r) vehicle.reset();
                if (event.key.keysym.sym == SDLK_p) paused = !paused;
            }
        }
    }

    void setPerspective() {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        const float nearPlane = 0.1f, farPlane = 500.0f;
        const float top = nearPlane * std::tan(65.0f * PI / 360.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-top * aspect, top * aspect, -top, top, nearPlane, farPlane);
        glMatrixMode(GL_MODELVIEW);
    }

    void lookAt(const Vec3& eye, const Vec3& target) {
        const Vec3 forward = normalized(target - eye);
        const Vec3 side = normalized(cross(forward, {0.0f, 1.0f, 0.0f}));
        const Vec3 up = cross(side, forward);
        const std::array<GLfloat, 16> matrix{
            side.x, up.x, -forward.x, 0.0f,
            side.y, up.y, -forward.y, 0.0f,
            side.z, up.z, -forward.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(matrix.data());
        glTranslatef(-eye.x, -eye.y, -eye.z);
    }

    void drawGround() {
        glDisable(GL_CULL_FACE);
        glColor3f(0.16f, 0.42f, 0.19f);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-180, -0.08f, -180); glVertex3f(180, -0.08f, -180);
        glVertex3f(180, -0.08f, 180); glVertex3f(-180, -0.08f, 180);
        glEnd();
        glEnable(GL_CULL_FACE);
    }

    void drawTrack() {
        constexpr int segments = 220;
        for (int i = 0; i < segments; ++i) {
            const float a0 = 2.0f * PI * static_cast<float>(i) / segments;
            const float a1 = 2.0f * PI * static_cast<float>(i + 1) / segments;
            const float c0 = std::cos(a0), s0 = std::sin(a0);
            const float c1 = std::cos(a1), s1 = std::sin(a1);
            const float nx0 = c0 / TRACK_X, nz0 = s0 / TRACK_Z;
            const float nx1 = c1 / TRACK_X, nz1 = s1 / TRACK_Z;
            const float len0 = std::sqrt(nx0*nx0 + nz0*nz0);
            const float len1 = std::sqrt(nx1*nx1 + nz1*nz1);
            const float ox0 = nx0 / len0 * ROAD_HALF_WIDTH;
            const float oz0 = nz0 / len0 * ROAD_HALF_WIDTH;
            const float ox1 = nx1 / len1 * ROAD_HALF_WIDTH;
            const float oz1 = nz1 / len1 * ROAD_HALF_WIDTH;

            glColor3f(0.19f, 0.19f, 0.21f);
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(TRACK_X*c0-ox0, 0.0f, TRACK_Z*s0-oz0);
            glVertex3f(TRACK_X*c0+ox0, 0.0f, TRACK_Z*s0+oz0);
            glVertex3f(TRACK_X*c1+ox1, 0.0f, TRACK_Z*s1+oz1);
            glVertex3f(TRACK_X*c1-ox1, 0.0f, TRACK_Z*s1-oz1);
            glEnd();

            if (i % 2 == 0) glColor3f(0.88f, 0.88f, 0.88f);
            else glColor3f(0.82f, 0.12f, 0.10f);
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(TRACK_X*c0-ox0*1.12f, 0.015f, TRACK_Z*s0-oz0*1.12f);
            glVertex3f(TRACK_X*c0-ox0, 0.015f, TRACK_Z*s0-oz0);
            glVertex3f(TRACK_X*c1-ox1, 0.015f, TRACK_Z*s1-oz1);
            glVertex3f(TRACK_X*c1-ox1*1.12f, 0.015f, TRACK_Z*s1-oz1*1.12f);
            glEnd();
            glBegin(GL_QUADS);
            glVertex3f(TRACK_X*c0+ox0, 0.015f, TRACK_Z*s0+oz0);
            glVertex3f(TRACK_X*c0+ox0*1.12f, 0.015f, TRACK_Z*s0+oz0*1.12f);
            glVertex3f(TRACK_X*c1+ox1*1.12f, 0.015f, TRACK_Z*s1+oz1*1.12f);
            glVertex3f(TRACK_X*c1+ox1, 0.015f, TRACK_Z*s1+oz1);
            glEnd();
        }

        glColor3f(0.95f, 0.95f, 0.95f);
        glBegin(GL_LINES);
        for (int i = 0; i < segments; i += 4) {
            const float a = 2.0f * PI * static_cast<float>(i) / segments;
            const float b = 2.0f * PI * static_cast<float>(i + 2) / segments;
            glVertex3f(TRACK_X*std::cos(a), 0.025f, TRACK_Z*std::sin(a));
            glVertex3f(TRACK_X*std::cos(b), 0.025f, TRACK_Z*std::sin(b));
        }
        glEnd();

        glPushMatrix();
        glTranslatef(TRACK_X + 0.2f, 0.04f, 0.0f);
        glRotatef(90.0f, 0, 1, 0);
        glColor3f(0.95f, 0.95f, 0.95f);
        glBegin(GL_QUADS);
        glVertex3f(-7, 0, -0.15f); glVertex3f(7, 0, -0.15f);
        glVertex3f(7, 0, 0.15f); glVertex3f(-7, 0, 0.15f);
        glEnd();
        glPopMatrix();
    }

    void drawCube(float sx, float sy, float sz) {
        const float x=sx/2, y=sy/2, z=sz/2;
        glBegin(GL_QUADS);
        glNormal3f(0,1,0); glVertex3f(-x,y,-z); glVertex3f(-x,y,z); glVertex3f(x,y,z); glVertex3f(x,y,-z);
        glNormal3f(0,-1,0); glVertex3f(-x,-y,z); glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z); glVertex3f(x,-y,z);
        glNormal3f(0,0,1); glVertex3f(-x,-y,z); glVertex3f(x,-y,z); glVertex3f(x,y,z); glVertex3f(-x,y,z);
        glNormal3f(0,0,-1); glVertex3f(x,-y,-z); glVertex3f(-x,-y,-z); glVertex3f(-x,y,-z); glVertex3f(x,y,-z);
        glNormal3f(1,0,0); glVertex3f(x,-y,z); glVertex3f(x,-y,-z); glVertex3f(x,y,-z); glVertex3f(x,y,z);
        glNormal3f(-1,0,0); glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z); glVertex3f(-x,y,z); glVertex3f(-x,y,-z);
        glEnd();
    }

    void drawCar() {
        glPushMatrix();
        glTranslatef(vehicle.position.x, vehicle.position.y, vehicle.position.z);
        glRotatef(-vehicle.yaw * 180.0f / PI, 0, 1, 0);
        glColor3f(0.82f, 0.055f, 0.045f);
        drawCube(2.0f, 0.65f, 4.0f);
        glPushMatrix();
        glTranslatef(0, 0.53f, -0.15f);
        glColor3f(0.08f, 0.16f, 0.22f);
        drawCube(1.48f, 0.65f, 1.8f);
        glPopMatrix();
        glColor3f(0.07f, 0.07f, 0.075f);
        for (float x : {-1.08f, 1.08f}) {
            for (float z : {-1.25f, 1.25f}) {
                glPushMatrix();
                glTranslatef(x, -0.16f, z);
                drawCube(0.34f, 0.62f, 0.82f);
                glPopMatrix();
            }
        }
        glPopMatrix();
    }

    void render() {
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setPerspective();
        glLoadIdentity();

        const Vec3 forward{std::sin(vehicle.yaw), 0.0f, -std::cos(vehicle.yaw)};
        const Vec3 eye = vehicle.position - forward * 10.0f + Vec3{0.0f, 5.4f, 0.0f};
        const Vec3 target = vehicle.position + forward * 5.0f + Vec3{0.0f, 0.7f, 0.0f};
        lookAt(eye, target);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        const GLfloat lightPosition[]{-0.45f, 1.0f, -0.35f, 0.0f};
        const GLfloat lightAmbient[]{0.22f, 0.23f, 0.26f, 1.0f};
        const GLfloat lightDiffuse[]{0.92f, 0.90f, 0.84f, 1.0f};
        const GLfloat lightSpecular[]{0.38f, 0.38f, 0.36f, 1.0f};
        const GLfloat materialSpecular[]{0.32f, 0.32f, 0.32f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 36.0f);
        drawGround();
        drawTrack();
        drawCar();
        glDisable(GL_LIGHTING);
        SDL_GL_SwapWindow(window);
    }

    void updateTitle() {
        const int kmh = static_cast<int>(std::abs(vehicle.speed) * 3.6f);
        const int remaining = std::max(0, 3 - vehicle.laps);
        char title[180];
        std::snprintf(title, sizeof(title),
            "Apex Racing Engine 0.2.0 | %03d km/h | Voltas: %d/3 | %s | WASD/Setas: dirigir | P: pausa | R: reset",
            kmh, vehicle.laps, paused ? "PAUSADO" : "CORRIDA");
        SDL_SetWindowTitle(window, title);
        (void)remaining;
    }
};
} // namespace apex

int main() {
    apex::Engine engine;
    if (!engine.initialize()) return 1;
    engine.run();
    return 0;
}
