step = 0.1;

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
I = eye(A);

sys = syslin('c', W);
sysd = dscr(sys, step);
Ad = sysd.A;
spec(Ad)
Hd = lyap(Ad, -I, 'd');
spec(Hd)
max(spec(Hd)) * step
