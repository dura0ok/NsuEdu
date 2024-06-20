// Параметры дискретизации
minValue = 0;
maxValue = 25;
step0 = 1;
step1 = 0.1;
step2 = 0.01;
step3 = 0.001;
count0 = (maxValue - minValue) / step0 + 1;
count1 = (maxValue - minValue) / step1 + 1;
count2 = (maxValue - minValue) / step2 + 1;
count3 = (maxValue - minValue) / step3 + 1;

n = 5;
T0 = 1.11;

Tcr = 6.787;
Kcr = 10.05;

K = Kcr - 0.52 + 0.02;
Ti = Tcr - 0.25 + 0.1;
Td = Ti / 4;
Ts = Td / 8;


s = poly(0, 's');
W0 = 1 / (1 + T0 * s)^n; 
W1 = (1 + 1 / (Ti * s) + (Td * s) / (Ts * s + 1)) * W0 * K; 
W = W1 / (1 + W1); 
b = 4.1171317 + 28.179319 * s +51.007142 * s^2 ; 
a = 4.1171318 + 31.040617 * s + 67.4808 * s^2 + 38.547717 * s^3 +46.443925 * s^4 +29.834491 * s^5 +9.3259598 * s^6 +s^7; 

A = [zeros(1, 6); eye(6, 6)];
ac = [4.1171318; 31.040617; 67.4808; 38.547717; 46.443925; 29.834491; 9.3259598];
A = [A, -ac];

B = [4.1171317; 28.179319; 51.007142; 0; 0; 0; 0]; 
C = [0 0 0 0 0 0 1]; 

function result = calculate_s(step)
	result = inv(A) * (eye(A) - expm(-A * step));
endfunction;

function result = calculate_ad(step)
	result = eye(A) + A * step;
endfunction;

function result = calculate_bd(Ad, S)
	result = Ad * S * B;
endfunction;

function result = calculate_error(realH, approxH)
	errors = (realH - approxH) .^ 2;
	errors_sum = sum(errors);
	result = errors_sum / length(realH);
endfunction;

h0 = read("1.txt", 1, count0); 
h1 = read("0,1.txt", 1, count1); 
h2 = read("0,01.txt", 1, count2); 
h3 = read("0,001.txt", 1, count3); 

function result = calculate_h(Ad, Bd, Cd, count)
	vk = zeros(Bd);
	h = [];
	for k = 1:count
		h = [h Cd * vk];
		vk = Ad * vk + Bd;
    end
    result = h;
endfunction;

sys0 = syslin('c', W);
sysd0 = dscr(sys0, step0);
Ad10 = sysd0.A;
Bd10 = sysd0.B;
Cd10 = sysd0.C;
h10 = calculate_h(Ad10, Bd10, Cd10, count0);

sys1 = syslin('c', W);
sysd1 = dscr(sys1, step1);
Ad11 = sysd1.A;
Bd11 = sysd1.B;
Cd11 = sysd1.C;
h11 = calculate_h(Ad11, Bd11, Cd11, count1);

sys2 = syslin('c', W);
sysd2 = dscr(sys2, step2);
Ad12 = sysd2.A;
Bd12 = sysd2.B;
Cd12 = sysd2.C;
h12 = calculate_h(Ad12, Bd12, Cd12, count2);

sys3 = syslin('c', W);
sysd3 = dscr(sys3, step3);
Ad13 = sysd3.A;
Bd13 = sysd3.B;
Cd13 = sysd3.C;
h13 = calculate_h(Ad13, Bd13, Cd13, count3);

Ad20 = calculate_ad(step0);
S0 = calculate_s(step0);
Bd20 = calculate_bd(Ad20, S0);
h20 = calculate_h(Ad20, Bd20, C, count0);

Ad21 = calculate_ad(step1);
S1 = calculate_s(step1);
Bd21 = calculate_bd(Ad21, S1);
h21 = calculate_h(Ad21, Bd21, C, count1);

Ad22 = calculate_ad(step2);
S2 = calculate_s(step2);
Bd22 = calculate_bd(Ad22, S2);
h22 = calculate_h(Ad22, Bd22, C, count2);

Ad23 = calculate_ad(step3);
S3 = calculate_s(step3);
Bd23 = calculate_bd(Ad23, S3);
h23 = calculate_h(Ad23, Bd23, C, count3);

calculate_error(h0, h10)
calculate_error(h0, h20)

calculate_error(h1, h11)
calculate_error(h1, h21)

calculate_error(h2, h12)
calculate_error(h2, h22)

calculate_error(h3, h13)
calculate_error(h3, h23)
