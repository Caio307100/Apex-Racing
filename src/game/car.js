import * as THREE from 'three';
export function createCar(scene){
 const car=new THREE.Group(),paint=new THREE.MeshStandardMaterial({color:0xe34b36,metalness:.62,roughness:.28}),dark=new THREE.MeshStandardMaterial({color:0x111820,metalness:.35,roughness:.42}),glass=new THREE.MeshStandardMaterial({color:0x172d3a,metalness:.55,roughness:.16});
 const body=new THREE.Mesh(new THREE.BoxGeometry(2.25,.58,4.5),paint);body.position.y=.72;body.castShadow=true;car.add(body);
 const cabin=new THREE.Mesh(new THREE.BoxGeometry(1.72,.66,2.05),glass);cabin.position.set(0,1.28,-.15);cabin.castShadow=true;car.add(cabin);
 const hood=new THREE.Mesh(new THREE.BoxGeometry(1.9,.12,1.25),paint);hood.position.set(0,1.03,1.25);car.add(hood);
 const spoiler=new THREE.Mesh(new THREE.BoxGeometry(2.35,.12,.38),dark);spoiler.position.set(0,1.28,-2.02);car.add(spoiler);
 const wheelGeo=new THREE.CylinderGeometry(.43,.43,.28,20);
 for(const x of [-1.12,1.12])for(const z of [-1.45,1.45]){const w=new THREE.Mesh(wheelGeo,dark);w.rotation.z=Math.PI/2;w.position.set(x,.48,z);w.castShadow=true;car.add(w);}
 const stripe=new THREE.Mesh(new THREE.BoxGeometry(.16,.025,3.8),new THREE.MeshStandardMaterial({color:0xf4eee0}));stripe.position.set(0,1.025,0);car.add(stripe);
 const lampMat=new THREE.MeshBasicMaterial({color:0xffe6b3});for(const x of [-.72,.72]){const lamp=new THREE.Mesh(new THREE.BoxGeometry(.42,.13,.08),lampMat);lamp.position.set(x,.85,2.27);car.add(lamp);}
 scene.add(car);return car;
}