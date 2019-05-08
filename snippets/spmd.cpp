void calc(int *in, int *out) {
  for (int i = 0; i < 1024; i++) {
    out[i] = in[i] * in[i];
  } 
}
calc(in, out);



void calc(int *in, int *out, int id) {
  out[id] = in[id] * in[id];
}

/* specify degree of parallelism */
parallel_for(calc, in, out, 1024);
