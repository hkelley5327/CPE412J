N = [100 250 500 750 1000 2500 5000 7500 ...
     10000 15000 20000 30000];

Timesteps = [100 500 1000];
Threads = [1 2 4 8 12 15];

SerialET = [
    0.005535  0.027718  0.055326
    0.034365  0.171363  0.342809
    0.136976  0.683953  1.372169
    0.309212  1.542580  3.086577
    0.547079  2.736175  5.482629
    3.421717  17.095797 34.210192
    13.675083 68.470012 136.810189
    30.779895 153.910367 307.793517
    54.749070 273.896922 548.876307
    123.636931 616.495694 1233.728477
    459.971387 2840.417392 2724.370439
    577.514066 3131.922273 6055.463128
];

[Ngrid, Tgrid] = ndgrid(N, Timesteps);

Serial = table( ...
    Ngrid(:), Tgrid(:), SerialET(:), ...
    'VariableNames', {'N', 'Timesteps', 'ET_s'});

% Parallel execution times: 100 timesteps
P100 = [
    0.006467 0.006119 0.005843 0.006305 0.006473 0.006926
    0.037612 0.023291 0.013003 0.012963 0.013434 0.013616
    0.149095 0.109399 0.073446 0.024188 0.024418 0.024269
    0.334934 0.310305 0.137646 0.102353 0.035899 0.035091
    0.594447 0.342798 0.178391 0.108551 0.111221 0.096491
    3.719216 2.168405 1.074429 0.551112 0.459218 0.429875
    14.875534 8.028026 3.845678 2.007274 1.367622 1.161564
    33.425016 19.010170 8.724145 4.443180 3.121495 2.361465
    59.571893 40.164261 15.167758 7.720850 5.220706 4.225399
    134.541645 70.533984 33.967220 17.288678 11.542302 9.241780
    627.326749 399.867260 141.279726 75.634292 45.643232 20.003666
    679.169946 397.909216 170.119287 82.810110 54.871694 45.932142
];

% Parallel execution times: 500 timesteps
P500 = [
    0.030371 0.027418 0.028256 0.028059 0.030096 0.031779
    0.185850 0.142560 0.062601 0.063100 0.064146 0.064793
    0.742487 0.616347 0.356553 0.117483 0.118808 0.118961
    1.675054 1.383068 0.673105 0.522449 0.175833 0.172166
    2.968687 1.672704 0.893819 0.482198 0.521070 0.459508
    18.570283 10.863307 5.530764 2.858741 2.259326 2.178792
    74.283744 38.784550 19.323989 9.893992 6.797998 5.802501
    167.165084 141.446725 43.026220 21.933649 15.201492 12.199648
    297.463640 210.747198 76.125610 38.370320 26.084148 21.225430
    673.266552 400.231880 169.443579 86.589685 58.217031 46.284741
    1727.107877 1487.960530 392.327638 188.074534 313.896888 163.403646
    3380.946635 2000.522042 851.041890 425.174569 274.623956 225.613078
];

% Parallel execution times: 1000 timesteps
P1000 = [
    0.060584 0.054512 0.055505 0.058118 0.059407 0.061827
    0.384930 0.289717 0.124869 0.126401 0.126621 0.128761
    1.486356 1.123572 0.688526 0.231171 0.237100 0.237760
    3.352796 2.853601 1.338059 0.916580 0.379813 0.338145
    5.956675 3.549676 1.773036 1.029581 1.055610 0.982248
    37.171339 22.410389 10.920099 5.841311 4.483648 4.247136
    148.866076 94.507524 38.414713 19.479300 13.597453 11.623642
    334.161078 218.524808 86.883093 44.096651 29.587610 24.613530
    594.474885 360.666765 150.671378 76.642990 52.489903 42.184700
    1349.166434 994.369653 339.247532 171.816924 115.073013 92.630558
    2976.289249 2308.083488 1292.203485 393.547444 247.129019 157.982734
    6590.768723 3674.100844 1726.687188 832.759454 552.084095 435.034685
];

ParallelET = zeros(12, 6, 3);

ParallelET(:,:,1) = P100;
ParallelET(:,:,2) = P500;
ParallelET(:,:,3) = P1000;

[Ngrid, ThreadGrid, Tgrid] = ndgrid(N, Threads, Timesteps);

Parallel = table( ...
    Ngrid(:), ...
    Tgrid(:), ...
    ThreadGrid(:), ...
    ParallelET(:), ...
    'VariableNames', {'N', 'Timesteps', 'Threads', 'ET_s'})

% Match serial execution times to parallel measurements
[~, idx] = ismember( ...
    [Parallel.N, Parallel.Timesteps], ...
    [Serial.N, Serial.Timesteps], ...
    'rows');

Parallel.Serial_ET_s = Serial.ET_s(idx);

% Calculate speedup
Parallel.Speedup = Parallel.Serial_ET_s ./ Parallel.ET_s;

% Calculate efficiency
Parallel.Efficiency = Parallel.Speedup ./ Parallel.Threads;

% Create final results table
Results = Parallel;

%% Serial Execution Time Graphs
figure;
hold on;

for ts = Timesteps

    subset = Serial(Serial.Timesteps == ts, :);

    plot(subset.N, subset.ET_s, '-o', ...
         'DisplayName', sprintf('%d Timesteps', ts));

end

xlabel('Number of Particles (N)');
ylabel('Execution Time (seconds)');
title('Serial Execution Time vs. Particle Count');
legend('Location', 'best');
grid on;
hold off;

saveas(gcf, 'Serial_Execution_Time.png');

%% Execution Time Graphs
for ts = Timesteps

    figure;
    hold on;

    for t = Threads
        subset = Results(Results.Timesteps == ts & ...
                         Results.Threads == t, :);

        plot(subset.N, subset.ET_s, '-o', ...
             'DisplayName', sprintf('%d Threads', t));
    end

    xlabel('Number of Particles (N)');
    ylabel('Execution Time (seconds)');
    title(sprintf('Execution Time vs. Particle Count (%d Timesteps)', ts));
    legend('Location', 'best');
    grid on;
    hold off;

    saveas(gcf, sprintf('Execution_Time_%d_Timesteps.png', ts));

end

%% Efficiency Graphs
for ts = Timesteps

    figure;
    hold on;

    for t = Threads
        subset = Results(Results.Timesteps == ts & ...
                         Results.Threads == t, :);

        plot(subset.N, subset.Efficiency, '-o', ...
             'DisplayName', sprintf('%d Threads', t));
    end

    xlabel('Number of Particles (N)');
    ylabel('Parallel Efficiency');
    title(sprintf('Efficiency vs. Particle Count (%d Timesteps)', ts));
    legend('Location', 'best');
    grid on;
    hold off;

    saveas(gcf, sprintf('Efficiency_%d_Timesteps.png', ts));

end

%% Speedup Graphs
for ts = Timesteps

    figure;
    hold on;

    for t = Threads
        subset = Results(Results.Timesteps == ts & ...
                         Results.Threads == t, :);

        plot(subset.N, subset.Speedup, '-o', ...
             'DisplayName', sprintf('%d Threads', t));
    end

    xlabel('Number of Particles (N)');
    ylabel('Parallel Speedup');
    title(sprintf('Speedup vs. Particle Count (%d Timesteps)', ts));
    legend('Location', 'best');
    grid on;
    hold off;

    saveas(gcf, sprintf('Speedup_%d_Timesteps.png', ts));

end