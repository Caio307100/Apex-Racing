import * as THREE from 'three';
export class Engine {
  constructor(canvas) {
    this.scene = new THREE.Scene(); this.scene.background = new THREE.Color(0x9db6c4); this.scene.fog = new THREE.Fog(0x9db6c4, 100, 360);
    this.camera = new THREE.PerspectiveCamera(65, 1, 0.1, 700);
    this.renderer = new THREE.WebGLRenderer({canvas, antialias:true, powerPreference:'high-performance'});
    this.renderer.setPixelRatio(Math.min(devicePixelRatio,1.8)); this.renderer.shadowMap.enabled=true; this.renderer.shadowMap.type=THREE.PCFSoftShadowMap;
    this.renderer.outputColorSpace=THREE.SRGBColorSpace; this.renderer.toneMapping=THREE.ACESFilmicToneMapping; this.renderer.toneMappingExposure=1.15;
    this.clock=new THREE.Clock(); this.systems=[]; this.resize(); addEventListener('resize',()=>this.resize());
  }
  addSystem(system){this.systems.push(system);}
  resize(){const w=innerWidth,h=innerHeight;this.camera.aspect=w/h;this.camera.updateProjectionMatrix();this.renderer.setSize(w,h,false);}
  start(){this.renderer.setAnimationLoop(()=>{const dt=Math.min(this.clock.getDelta(),0.04);for(const system of this.systems)system.update(dt);this.renderer.render(this.scene,this.camera);});}
}