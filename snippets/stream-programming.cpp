int main (){

for(std::size_t i = 0; i < std::size(vec_c); ++i) {
    vec_c[i] = vec_a[i] + vec_b[i];
}


int my_thread_id = whoami();
vec_c[my_thread_id] = vec_a[my_thread_id] + vec_b[my_thread_id];

}