cities = [5:1:15]
ET = [4e-6 7e-6 1.8e-5 9.6e-5 0.000784 0.006719 0.072038 0.817469 10.2234 139.401 2015.66] % in seconds

figure;

semilogy(cities, ET, '-o', 'LineWidth', 1.5, 'MarkerSize', 6);
grid on
title('Travelling Salesman Problem Execution Time vs. Number of Cities');
xlabel('Number of Cities');
ylabel('Execution Time (sec)');
xticks(cities);