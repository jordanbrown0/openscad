#version 120

void main() {
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
