D = 0;

% Define the flow field
theta_dot = @(theta, v) (v - cos(theta)./v);
v_dot = @(theta, v)  (-sin(theta) - D*v.^2);

% Energy functional
L = @(theta, v) v.^3 - 3*v.*cos(theta);

% Define the grids
N_theta = 15; theta_lim = [-pi, pi];
theta_range = linspace(theta_lim(1), theta_lim(2), N_theta);
theta_range_C = linspace(theta_lim(1), theta_lim(2), 100);

N_v = 10; v_lim = [(1+D^2)^(-1/4) - 0.6, (1+D^2)^(-1/4) + 0.6];
v_range = linspace(v_lim(1), v_lim(2), N_v);
v_range_C = linspace(v_lim(1)-0.2, v_lim(2)+0.2, 100);

[Theta, V] = meshgrid(theta_range, v_range);
[Theta_C, V_C] = meshgrid(theta_range_C, v_range_C);

% Compute the flow fields
Theta_dot = theta_dot(Theta, V);
V_dot = v_dot(Theta, V);

figure();
% Plot the flow field
quiver(Theta, V, Theta_dot, V_dot, 'r');
hold on
% Plot the contour of the energy functional
contour(Theta_C, V_C, L(Theta_C, V_C), 100);
% axis equal
hold off