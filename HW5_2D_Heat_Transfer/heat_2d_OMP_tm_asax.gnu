set output "heat_2d_OMP_tm_asax.png"
set term png
set title "Execution Time, 2D Heat Transfer -- ASA-X System"
set grid
set xrange[1024:45000]
set logscale x 2
set xlabel "n, Active Data Points in X and Y Dimension";
set ylabel "Execution Time in Seconds";
set key center top;
plot "heat_2d_serial_tm.txt" with linespoints t "Serial Ref","heat_2d_OMP_1_tm.txt" with linespoints t "1 OMP Threads","heat_2d_OMP_2_tm.txt" with linespoints t "2 OMP Threads","heat_2d_OMP_4_tm.txt" with linespoints t "4 OMP Threads", "heat_2d_OMP_8_tm.txt" with linespoints t "8 OMP Threads","heat_2d_OMP_16_tm.txt" with linespoints t "16 OMP Threads";
