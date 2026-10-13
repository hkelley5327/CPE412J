set output "heat_2d_OMP_ef_asax.png"
set term png
set title "Efficiency, 2D Heat Transfer -- ASA-X System"
set grid
set xrange [1024:45000]
set logscale x 2
set xlabel "n, Active Data Points in X and Y Dimension";
set ylabel "Efficiency";
set key right bottom;
plot "heat_2d_OMP_1_ef.txt" with linespoints t "1 OMP Threads","heat_2d_OMP_2_ef.txt" with linespoints t "2 OMP Threads","heat_2d_OMP_4_ef.txt" with linespoints t "4 OMP Threads", "heat_2d_OMP_8_ef.txt" with linespoints t "8 OMP Threads","heat_2d_OMP_16_ef.txt" with linespoints t "16 OMP Threads";
