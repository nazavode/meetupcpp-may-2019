std::transform(std::begin(a), std::end(a),
               std::begin(b), std::begin(r), std::plus<>{});

#include <execution>

std::transform(std::execution::par_unseq, 
               std::begin(a), std::end(a),
               std::begin(b), std::begin(r), std::plus<>{});


sycl::buffer<int> ka{std::data(a), std::size(a));
sycl::buffer<int> kb{std::data(b), std::size(b));
sycl::buffer<int> kr{std::data(r), std::size(r));
sycl::queue q;
sycl::sycl_execution_policy<class VectorAdd> sycl_policy{q};

std::transform(sycl_policy, std::begin(ka), std::end(ka), 
               std::begin(kb), std::begin(kr), std::plus<>{});
