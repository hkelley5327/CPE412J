% from last HW Serial
cities_serial = 5:15;
ET_serial = [4e-6, 7e-6, 1.8e-5, 9.6e-5, 0.000784, 0.006719, 0.072038, 0.817469, 10.2234, 139.401, 2015.66];

% omp
cities_omp = 5:15;
ET_omp = [0.000298, 0.000283, 0.000334, 0.00042, 0.000686, 0.001657, 0.008125, 0.080284, 0.908653, 11.2761, 150.868];

% OMP Performance Metrics

p_omp = cities_omp - 1;

[eff_omp, speedup_omp, cost_omp] = performance_metrics(ET_serial, p_omp, ET_omp);

results_table_omp = table(cities_omp', p_omp', ET_serial', ET_omp', ...
    speedup_omp', eff_omp', cost_omp', ...
    'VariableNames', {'N_Cities', 'Threads_p', 'T_Serial_s', ...
    'T_Parallel_s', 'Speedup', 'Efficiency', 'Cost_s'});
disp('--- Performance Metrics for OMP ---');
disp(results_table_omp);

% OMP Graphs

% Execution Time
figure;
semilogy(cities_omp, ET_omp, '-o', 'LineWidth', 2, ...
    'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Parallel Execution Time vs. Number of Cities (OMP)');
xlabel('Number of Cities (N)');
ylabel('Execution Time (s)');
xticks(cities_omp);
saveas(gcf, 'execution_time_OMP.png');


% Speedup vs. Number of Cities
figure;
plot(cities_omp, speedup_omp, '-o', 'LineWidth', 2, ...
    'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Speedup vs. Number of Cities (OMP)');
xlabel('Number of Cities (N)');
ylabel('Speedup S(N)');
xticks(cities_omp);
saveas(gcf, 'speedup_OMP.png');


% Efficiency vs. Number of Cities
figure;
plot(cities_omp, eff_omp, '-o', 'LineWidth', 2, ...
    'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Efficiency vs. Number of Cities (OMP)');
xlabel('Number of Cities (N)');
ylabel('Efficiency E(N)');
xticks(cities_omp);
ylim([0 1.5]);
saveas(gcf, 'efficiency_OMP.png');


% Cost vs. Number of Cities
figure;
semilogy(cities_omp, cost_omp, '-o', 'LineWidth', 2, ...
    'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Cost vs. Number of Cities (OMP)');
xlabel('Number of Cities (N)');
ylabel('Cost C(N) (Core-Seconds)');
xticks(cities_omp);
saveas(gcf, 'cost_OMP.png');


% Helper Function
function [rel_eff, rel_spdup, cost] = performance_metrics(ts, p, tp)

    rel_spdup = ts ./ tp;
    rel_eff   = rel_spdup ./ p;
    cost      = p .* tp;

end