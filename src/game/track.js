import * as THREE from 'three';
export function createTrack(scene){
 const group=new THREE.Group(),points=[],rx=86,rz=57;
 for(let i=0;i<=240;i++){const a=i/240*Math.PI*2;points.push(new THREE.Vector3(Math.cos(a)*rx,0,Math.sin(a)*rz));}
 const curve=new THREE.CatmullRomCurve3(points,true);
 const road=new THREE.Mesh(new THREE.TubeGeometry(curve,360,13,8,true),new THREE.MeshStandardMaterial({color:0x252b31,roughness:.92}));road.position.y=.05;road.receiveShadow=true;group.add(road);
 const edgeMat=new THREE.MeshStandardMaterial({color:0xe8e6d9});
 for(const offset of [-11.7,11.7]){const p=[];for(let i=0;i<240;i++){const a=i/240*Math.PI*2;p.push(new THREE.Vector3(Math.cos(a)*(rx+offset),.16,Math.sin(a)*(rz+offset)));}group.add(new THREE.Mesh(new THREE.TubeGeometry(new THREE.CatmullRomCurve3(p,true),360,.22,5,true),edgeMat));}
 const grass=new THREE.Mesh(new THREE.PlaneGeometry(600,600),new THREE.MeshStandardMaterial({color:0x49634c,roughness:1}));grass.rotation.x=-Math.PI/2;grass.position.y=-.12;grass.receiveShadow=true;group.add(grass);
 const grid=new THREE.GridHelper(600,60,0x58745a,0x58745a);grid.position.y=-.1;group.add(grid);
 for(let i=0;i<22;i++)for(const x of [-12.8,12.8]){const curb=new THREE.Mesh(new THREE.BoxGeometry(1.1,.24,2.5),new THREE.MeshStandardMaterial({color:i%2?0xf1eee4:0xd84a40}));curb.position.set(x,.16,30-i*2.6);group.add(curb);}
 scene.add(group);return{group,curve,rx,rz};
}
export function createEnvironment(scene){
 scene.add(new THREE.HemisphereLight(0xdcecff,0x42513a,2));
 const sun=new THREE.DirectionalLight(0xffe4bd,3.2);sun.position.set(-70,110,55);sun.castShadow=true;sun.shadow.mapSize.set(2048,2048);sun.shadow.camera.left=-140;sun.shadow.camera.right=140;sun.shadow.camera.top=140;sun.shadow.camera.bottom=-140;scene.add(sun);
 const disc=new THREE.Mesh(new THREE.SphereGeometry(7,24,16),new THREE.MeshBasicMaterial({color:0xffe6bd}));disc.position.set(-180,130,-220);scene.add(disc);
}