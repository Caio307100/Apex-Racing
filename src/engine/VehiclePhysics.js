import * as THREE from 'three';
// Dinâmica veicular arcade: aceleração, arrasto, frenagem, esterçamento e aderência simplificada.
export class VehiclePhysics {
  constructor(){this.position=new THREE.Vector3(0,0.55,42);this.velocity=0;this.heading=Math.PI;this.steer=0;this.maxSpeed=62;}
  update(dt,input){
    const throttle=input.forward?1:0,brake=input.backward?1:0,steerInput=(input.right?1:0)-(input.left?1:0);
    this.velocity=THREE.MathUtils.clamp(this.velocity+(throttle*30-brake*48-this.velocity*0.48)*dt,-12,this.maxSpeed);
    this.steer=THREE.MathUtils.damp(this.steer,steerInput,7,dt);
    const grip=THREE.MathUtils.clamp(Math.abs(this.velocity)/9,0,1);
    this.heading-=this.steer*grip*(0.9+Math.min(Math.abs(this.velocity)/42,0.8))*dt*Math.sign(this.velocity||1);
    this.position.x+=Math.sin(this.heading)*this.velocity*dt;this.position.z+=Math.cos(this.heading)*this.velocity*dt;
  }
  reset(){this.position.set(0,0.55,42);this.velocity=0;this.heading=Math.PI;this.steer=0;}
}