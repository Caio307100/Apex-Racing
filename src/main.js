import * as THREE from 'three';
import {Engine} from './engine/Engine.js';import {VehiclePhysics} from './engine/VehiclePhysics.js';import {createTrack,createEnvironment} from './game/track.js';import {createCar} from './game/car.js';import './style.css';
const engine=new Engine(document.querySelector('#game')),{scene,camera}=engine;createEnvironment(scene);const track=createTrack(scene),car=createCar(scene),physics=new VehiclePhysics();
const input={forward:false,backward:false,left:false,right:false},map={KeyW:'forward',ArrowUp:'forward',KeyS:'backward',ArrowDown:'backward',KeyA:'left',ArrowLeft:'left',KeyD:'right',ArrowRight:'right'};
let started=false,finished=false,elapsed=0,lap=1,previousAngle=null,lastAngle=null;const $=id=>document.getElementById(id);
function resetCar(){physics.reset();car.position.copy(physics.position);car.rotation.y=physics.heading;}
function key(e,down){const action=map[e.code];if(action){input[action]=down;e.preventDefault();}if(e.code==='KeyR'&&down)resetCar();}
addEventListener('keydown',e=>key(e,true));addEventListener('keyup',e=>key(e,false));addEventListener('blur',()=>Object.keys(input).forEach(k=>input[k]=false));
function startRace(){started=true;finished=false;elapsed=0;lap=1;previousAngle=null;lastAngle=null;$('message').classList.add('hidden');$('finish').classList.remove('visible');resetCar();}
$('start-button').addEventListener('click',startRace);$('again-button').addEventListener('click',startRace);
function time(t){const m=Math.floor(t/60),s=Math.floor(t%60),ms=Math.floor(t%1*1000);return String(m).padStart(2,'0')+':'+String(s).padStart(2,'0')+'.'+String(ms).padStart(3,'0');}
function updateRace(dt){
 if(!started||finished)return;elapsed+=dt;physics.update(dt,input);
 const radial=Math.hypot(physics.position.x/track.rx,physics.position.z/track.rz),error=Math.abs(radial-1);
 if(error>.17)physics.velocity*=Math.max(0,1-dt*1.7);if(error>.42)physics.velocity*=Math.max(0,1-dt*4.5);
 car.position.copy(physics.position);car.rotation.y=physics.heading;
 const angle=Math.atan2(physics.position.z/track.rz,physics.position.x/track.rx);
 if(previousAngle!==null){let delta=angle-previousAngle;if(delta>Math.PI)delta-=Math.PI*2;if(delta< -Math.PI)delta+=Math.PI*2;if(delta<-.012&&lastAngle!==null&&lastAngle>2.7&&angle< -2.7){lap++;if(lap>3){finished=true;$('final-time').textContent='Tempo total: '+time(elapsed);$('finish').classList.add('visible');}}}
 previousAngle=angle;if(Math.abs(angle)>2.7)lastAngle=angle;
 $('speed').textContent=String(Math.round(Math.abs(physics.velocity)*3.6)).padStart(3,'0');$('speed-fill').style.width=Math.min(100,Math.abs(physics.velocity)/physics.maxSpeed*100)+'%';$('lap').textContent=String(Math.min(lap,3)).padStart(2,'0');$('timer').textContent=time(elapsed);
}
function updateCamera(dt){const forward=new THREE.Vector3(Math.sin(physics.heading),0,Math.cos(physics.heading)),desired=car.position.clone().addScaledVector(forward,-10).add(new THREE.Vector3(0,5.4,0));camera.position.lerp(desired,1-Math.exp(-4.2*dt));camera.lookAt(car.position.clone().addScaledVector(forward,5).add(new THREE.Vector3(0,1.1,0)));}
engine.addSystem({update(dt){updateRace(dt);updateCamera(dt);}});resetCar();engine.start();