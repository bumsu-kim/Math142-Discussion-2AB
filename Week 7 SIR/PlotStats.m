%% load data
fname = 'output_N_1000000_I0_0.0001_timeUnit_0.5_gamma_0.166667_avgContact_2_TransmitChance_0.125_time_period_150.csv';
dat = readmatrix(fname); % skips the header (1st row)

EpidemicEnd = find(dat(:,3)==0, 1, 'first');
N = 100000; % total population
T = dat(1:EpidemicEnd, 1);
S = dat(1:EpidemicEnd, 2)/N; % fraction
I = dat(1:EpidemicEnd, 3)/N; % fraction
R = dat(1:EpidemicEnd, 4)/N; % fraction

newInfections = dat(:, 5);
theoretical_betaSI = dat(:,6);

%% Plot SIR
figure(); hold on;
plot(T, S, 'LineWidth',2); 
plot(T, I, 'LineWidth',2);
plot(T, R, 'LineWidth',2);
legend('S', 'I', 'R', 'Location','best')
xlabel('Time (days)');
ylabel('Fraction');
title('S I R over time')

%% Compare with theory (traditional SIR model)
%
% beta = (contacts per person per time) * (chance to transmit per contact)
% gamma = (1 / average recovery time )
% 
% Traditional SIR Model assumes
%
% S' = -beta * S * I
% I' =  beta * S * I - gamma * I
% R' = gamma * I
%
% We will see if the number of new infections (-S') at each time step
%  actually matches with beta*S*I.
%
% (R' being gamma*I is more straightforward from the simulation setting)

figure(); hold on;
[~, peak_idx] = max(I);
% You will see two apparently different curves, where the data come from
% before the peak of I (up) and after the peak of I (down), respectively.
% So we separate the data into two parts and analyze them

bSI_up = theoretical_betaSI(1:peak_idx);
bSI_down = theoretical_betaSI(peak_idx:end);
nI_up = newInfections(1:peak_idx);
nI_down = newInfections(peak_idx:end);
% need "Curve Fitting" Toolbox
[curve_up, gof_up] = fit(bSI_up, nI_up, 'poly1');
[curve_down, gof_down] = fit(bSI_down, nI_down, 'poly1');
plot(bSI_up, nI_up, 'b');
plot(bSI_down, nI_down, 'r');
plot(curve_up, 'b:');
plot(curve_down, 'r:');
legend('Up', 'Down', 'Up fitting', 'Down fitting', 'Location', 'best');
xlabel('Theoretical \beta S I');
ylabel('Actual New Infections Each Step');
title('\beta S I vs New Infections')
disp('Fitting for before peak: ')
disp(curve_up);
disp('Fitting for after peak: ')
disp(curve_down);