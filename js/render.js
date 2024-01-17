FOV = 2000.0;
cube_rotation = {x:0, y:0, z:0};
cam_pos = {x:0, y:0, z:-5};

cube_vertices = [
    { x : -1, y : -1, z : -1 }, // 1
    { x : -1, y :  1, z : -1 }, // 2
    { x :  1, y :  1, z : -1 }, // 3
    { x :  1, y : -1, z : -1 }, // 4
    { x :  1, y :  1, z :  1 }, // 5
    { x :  1, y : -1, z :  1 }, // 6
    { x : -1, y :  1, z :  1 }, // 7
    { x : -1, y : -1, z :  1 }  // 8
];

cube_faces = [
    // front
    { a : 1, b : 2, c : 3, color : "#FF0000" },
    { a : 1, b : 3, c : 4, color : "#FF0000" },
    // right
    { a : 4, b : 3, c : 5, color : "#00FF00" },
    { a : 4, b : 5, c : 6, color : "#00FF00" },
    // back
    { a : 6, b : 5, c : 7, color : "#0000FF" },
    { a : 6, b : 7, c : 8, color : "#0000FF" },
    // left
    { a : 8, b : 7, c : 2, color : "#FFFF00" },
    { a : 8, b : 2, c : 1, color : "#FFFF00" },
    // top
    { a : 2, b : 7, c : 5, color : "#FF00FF" },
    { a : 2, b : 5, c : 3, color : "#FF00FF" },
    // bottom
    { a : 6, b : 8, c : 1, color : "#00FFFF" },
    { a : 6, b : 1, c : 4, color : "#00FFFF" }
];

render_triangles = [];

background_color = "#474747";
line_color = "#383838";
cube_line_color = "#9ba8a8"
vtx_color = "#a3c4c4"
// SETUP 
window_height = 200;
window_width = 200;

var buffer = Array(window_height*window_width).fill(background_color);

const canvas = document.getElementById("render-view");
const ctx = canvas.getContext("2d");
canvas.height = window_height;
canvas.width = window_width;

//
// VECTOR OPERATIONS 
function rotate_x(v, angle) {
    rotated_vector = {
        x : v.x,
        y : v.y * Math.cos(angle) - v.z * Math.sin(angle),
        z : v.y * Math.sin(angle) + v.z * Math.cos(angle)
    };
    return rotated_vector;
}

function rotate_y(v, angle) {
    rotated_vector = {
        x : v.x * Math.cos(angle) - v.z * Math.sin(angle),
        y : v.y,
        z : v.x * Math.sin(angle) + v.z * Math.cos(angle)
    };
    return rotated_vector;
}

function rotate_z(v, angle) {
    rotated_vector = {
        x : v.x * Math.cos(angle) - v.y * Math.sin(angle),
        y : v.x * Math.sin(angle) + v.y * Math.cos(angle),
        z : v.z
    };

    return rotated_vector;
}


// DISPLAY FUNCTIONS 
function clear_render(color){
    ctx.fillStyle = color;
    ctx.fillRect(0,0, window_height, window_width);

    buffer.fill(background_color); 
}

function paint_pixel(x,y, color){
    buffer[(window_width * y) + x] = color;
}

function draw_box(x, y, width, height, color) {
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            current_x = x + i;
            current_y = y + j;
            paint_pixel(Math.round(current_x), Math.round(current_y), color);
        }
    }
}

function draw_line(x0, y0, x1, y1, color){
  // DDA algorithm 
  delta_x = (x1 - x0);
  delta_y = (y1 - y0);
  
  side_length = Math.abs(delta_x) >= Math.abs(delta_y) ? Math.abs(delta_x) : Math.abs(delta_y);
  
  // find how much we need to increment
  x_inc = delta_x /side_length;
  y_inc = delta_y /side_length;
  
  current_x = x0;
  current_y = y0;

  for (i = 0; i <= side_length; i++){
      paint_pixel(Math.round(current_x), Math.round(current_y), color);
      current_x += x_inc;
      current_y += y_inc;
  }
}
function draw_triangle(x0, y0, x1, y1, x2, y2, color){
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

function draw_grid(color, spacing){
     for (y = 0; y < window_height; y++){
        for(x=0; x < window_width; x++){
            if (x%spacing == 0 || x==window_width-1)
                buffer[(window_width * y) + x] = color;
            else if (y%spacing  == 0 || y==window_height-1)
                buffer[(window_width * y) + x] = color;
        }
    }
}

function project(vtx){
    pVtx = {
       x : ((FOV * vtx.x) / vtx.z), 
       y : ((FOV * vtx.y) / vtx.z) 
    }
    
    return pVtx;
}

function update(){

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;
    
    cube_faces.forEach(face => {
        vtxList = [];
        vtxList.push(cube_vertices[face.a-1]);
        vtxList.push(cube_vertices[face.b-1]);
        vtxList.push(cube_vertices[face.c-1]);
        
        // transform
        transformedVtxList = [];
        vtxList.forEach(vtx => {
           trfVtx = rotate_x(vtx, cube_rotation.x); 
           trfVtx = rotate_y(trfVtx, cube_rotation.y); 
           trfVtx = rotate_z(trfVtx, cube_rotation.z); 
           trfVtx.z += 45;
           transformedVtxList.push(trfVtx);
        });
        
        projectedVtx = [];
        // project
        transformedVtxList.forEach(vtx => {
            pVtx = project(vtx);
            pVtx.x += (window_width/2.0);
            pVtx.y += (window_height/2.0);
            projectedVtx.push(pVtx);
        });

        render_triangles.push(projectedVtx);       
    });
   

}
// RENDERING 
function draw_canvas(){
    for (y = 0; y < window_height; y++){
        for(x=0; x < window_width; x++){
            color = buffer[(window_width * y) + x]
            ctx.fillStyle = color;
            ctx.fillRect(x,y, 1,1);
        }
    }
}

// RENDER!
function render(){
    clear_render(background_color);
    
    draw_grid(line_color, 10);
    render_triangles.forEach(triangle => {
        draw_triangle(triangle[0].x, triangle[0].y, 
                      triangle[1].x, triangle[1].y,
                      triangle[2].x, triangle[2].y,
                      cube_line_color
                    )
    });

    render_triangles.forEach(triangle => {
        draw_box(triangle[0].x, triangle[0].y, 3, 3, vtx_color);
        draw_box(triangle[1].x, triangle[1].y, 3, 3, vtx_color);
        draw_box(triangle[2].x, triangle[2].y, 3, 3, vtx_color);
    }); 
    draw_canvas();
    render_triangles = [];
}


setInterval(onTimerTick, 33);
function onTimerTick(){
    update();
    render();   
}

