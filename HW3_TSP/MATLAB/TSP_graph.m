% PART 1 Serial
cities_serial = 5:15;
ET_serial = [4e-6, 7e-6, 1.8e-5, 9.6e-5, 0.000784, 0.006719, 0.072038, 0.817469, 10.2234, 139.401, 2015.66];
ET_serial_short = [4e-6, 7e-6, 1.8e-5, 9.6e-5, 0.000784, 0.006719, 0.072038, 0.817469, 10.2234, 139.401];

% Serial Plots
% Execution Time
figure;
semilogy(cities_serial, ET_serial, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b')
grid on;
title('Serial Execution Time vs. Number of Cities');
xlabel('Number of Cities (N)');
ylabel('Execution Time (s)');
xticks(cities_serial);
saveas(gcf, 'execution_time_serial.png');

% PART 2 PTH1
cities_PTH1 = 5:14;
ET_PTH1 = [0.000216, 0.000252, 0.000261, 0.000543, 0.004044, 0.037121, 0.365174, 4.20847, 56.0196, 670.941];
p_PTH1 = cities_PTH1 - 1; 

[rel_eff_PTH1, rel_spdup_PTH1, cost_PTH1] = performance_metrics(ET_serial_short, p_PTH1, ET_PTH1);

results_table_PTH1 = table(cities_PTH1', p_PTH1', ET_serial_short', ET_PTH1', rel_spdup_PTH1', rel_eff_PTH1', cost_PTH1', ...
    'VariableNames', {'N_Cities', 'Threads_p', 'T_Serial_s', 'T_Parallel_s', 'Speedup', 'Efficiency', 'Cost_s'});
disp('--- Performance Metrics for PTH1 ---');
disp(results_table);

% PTH1 Graphs
% Execution Time
figure;
semilogy(cities_PTH1, ET_PTH1, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b')
grid on;
title('Paralell Execution Time vs. Number of Cities (PTH1)');
xlabel('Number of Cities (N)');
ylabel('Execution Time (s)');
xticks(cities_PTH1);
saveas(gcf, 'execution_time_PTH1.png');

% Speedup vs. Number of Cities
figure;
plot(cities_PTH1, rel_spdup_PTH1, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Speedup vs. Number of Cities (PTH1)');
xlabel('Number of Cities (N)');
ylabel('Speedup S(N)');
xticks(cities_PTH1);
saveas(gcf, 'speedup_PTH1.png');

% Efficiency vs. Number of Cities
figure;
plot(cities_PTH1, rel_eff_PTH1, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Efficiency vs. Number of Cities (PTH1)');
xlabel('Number of Cities (N)');
ylabel('Efficiency E(N)');
xticks(cities_PTH1);
ylim([0 0.03]); 
saveas(gcf, 'efficiency_PTH1.png');

% Cost vs. Number of Cities
figure;
semilogy(cities_PTH1, cost_PTH1, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Cost vs. Number of Cities (PTH1)');
xlabel('Number of Cities (N)');
ylabel('Cost C(N) (Core-Seconds)');
xticks(cities_PTH1);
saveas(gcf, 'cost_PTH1.png');

% PART 3 PTH2
cities_PTH2 = 5:15;
ET_PTH2 = [0.000387 0.000352 0.000361 0.000434 0.000579 0.001595 0.008654 0.081611 0.910806 11.2759 152.662];
p_PTH2 = cities_PTH2 - 1; 

[rel_eff_PTH2, rel_spdup_PTH2, cost_PTH2] = performance_metrics(ET_serial, p_PTH2, ET_PTH2);

results_table_PTH2 = table(cities_PTH2', p_PTH2', ET_serial', ET_PTH2', rel_spdup_PTH2', rel_eff_PTH2', cost_PTH2', ...
    'VariableNames', {'N_Cities', 'Threads_p', 'T_Serial_s', 'T_Parallel_s', 'Speedup', 'Efficiency', 'Cost_s'});
disp('--- Performance Metrics for PTH2 ---');
disp(results_table);

% PTH2 Graphs
% Execution Time
figure;
semilogy(cities_PTH2, ET_PTH2, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Parallel Execution Time vs. Number of Cities (PTH2)');
xlabel('Number of Cities (N)');
ylabel('Execution Time (s)');
xticks(cities_PTH2);
saveas(gcf, 'execution_time_PTH2.png');

% Speedup vs. Number of Cities
figure;
plot(cities_PTH2, rel_spdup_PTH2, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Speedup vs. Number of Cities (PTH2)');
xlabel('Number of Cities (N)');
ylabel('Speedup S(N)');
xticks(cities_PTH2);
saveas(gcf, 'speedup_PTH2.png');

% Efficiency vs. Number of Cities
figure;
plot(cities_PTH2, rel_eff_PTH2, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Efficiency vs. Number of Cities (PTH2)');
xlabel('Number of Cities (N)');
ylabel('Efficiency E(N)');
xticks(cities_PTH2);
ylim([0 1.1]); % Expanded to standard efficiency range [0, 1.1]
saveas(gcf, 'efficiency_PTH2.png');

% Cost vs. Number of Cities
figure;
semilogy(cities_PTH2, cost_PTH2, '-o', 'LineWidth', 2, 'MarkerSize', 8, 'MarkerFaceColor', 'b');
grid on;
title('Cost vs. Number of Cities (PTH2)');
xlabel('Number of Cities (N)');
ylabel('Cost C(N) (Core-Seconds)');
xticks(cities_PTH2);
saveas(gcf, 'cost_PTH2.png');

% Helper Function
function [rel_eff, rel_spdup, cost] = performance_metrics(ts, p, tp)
    rel_spdup = ts ./ tp;
    rel_eff   = rel_spdup ./ p;
    cost      = p .* tp;
end