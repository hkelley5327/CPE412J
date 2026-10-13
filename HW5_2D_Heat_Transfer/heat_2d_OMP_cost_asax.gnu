set output "heat_2d_OMP_cost_asax.png"
set term png
set title "Time Costs, 2D Heat Transfer -- ASA-X System"
set grid
set xrange [1024:45000]
set logscale x 2
set xlabel "n, Active Data Points in X and Y Dimension";
set ylabel "Time Cost in Aggregate Seconds";
set key left top;
plot "heat_2d_serial_tm.txt" with linespoints t "Serial Ref","heat_2d_OMP_1_ct.txt" with linespoints t "1 OMP Threads","heat_2d_OMP_2_ct.txt" with linespoints t "2 OMP Threads","heat_2d_OMP_4_ct.txt" with linespoints t "4 OMP Threads", "heat_2d_OMP_8_ct.txt" with linespoints t "8 OMP Threads","heat_2d_OMP_16_ct.txt" with linespoints t "16 OMP Threads";
