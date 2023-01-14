import numpy as np
import matplotlib.pyplot as plt
import scipy.linalg as sl
import cv2
import matplotlib.animation as animation

g = 9.81
l = 0.5
b = 0.1

# x = [theta omega]
A = np.array([
    [0, 1],
    [g/l, b]])

B = np.array([
    [0],
    [1]])

Q = np.array([
    [1, 0],
    [0, 1]])

R = np.array([[1]])

L = (sl.solve_continuous_are(A, B, Q, R))  # ricatti solution
K = np.matmul(np.matmul(np.linalg.inv(R), B.copy().T),
              L)  # k = R(inverse)B(transpose)L
M = A-np.matmul(B, K)  # M = A-BK

theta = (np.pi) - 0.1  # setting up initial angle
omega = 0  # setting up initial omega
t = 0
time = [0]
theta_vals = [theta]  # it will store all values of theta
omega_vals = [omega]  # it will store all values of omega
print(K)
# simulating till 10 sec
# couldnt take into account torq max condition :(
while (t <= 10):
    # torque = u matrix = -k(x-[pi 0])
    u = -np.matmul(K, np.array([[theta - np.pi], [omega]]))
    if np.asscalar(u) > 10:
        u = np.array([[10]])
    if np.asscalar(u) < -10:
        u = np.array([[-10]])
    torq = np.asscalar(u)
    y = np.matmul(A, np.array([[theta - np.pi], [omega]])
                  ) + np.matmul(B, u)  # y = M(x-[pi 0])
    theta = theta + omega*0.01  # next theta = prev theta + omega*timestep
    # next omega = prev omega + ang acceleration*timestep
    omega = omega + 0.01*np.asscalar(y[1])
    theta_vals.append(theta)
    omega_vals.append(omega)
    t += 0.01
    time.append(t)

# plotting
plt.plot(time, theta_vals, label="theta")
plt.plot(time, omega_vals, label="omega")
plt.show()
# animation

fig = plt.figure(figsize=(5, 5), facecolor='w')
ax = fig.add_subplot(1, 1, 1)
plt.rcParams['font.size'] = 15

lns = []
for i in range(len(time)):
    ln, = ax.plot([0, np.sin(theta_vals[i])], [0, -np.cos(theta_vals[i])],
                  color='k', lw=2)
    tm = ax.text(-1, 0.9, 'time = %.1fs' % time[i])
    lns.append([ln, tm])

ax.set_aspect('equal', 'datalim')
ax.grid()
ani = animation.ArtistAnimation(fig, lns, interval=5)
plt.show()
