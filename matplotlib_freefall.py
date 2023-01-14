# simulate the free-fall of the pendulum
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# angular acceleration = -(g/l)sin theta   (free fall, so torque = 0)
g = 9.81
l = 2  # say

# releasing from extreme position
theta_not = np.pi/2  # initial angle theta
omega_not = 0  # initial angular velocity

# let us plot till t=10
time_step = 0.01  # timesteps of our simulation
# getting time points after each timestep
timings = [0.01*i for i in range(1001)]

t = 0
theta = [np.pi/2]  # stores all values of theta at each timestep
omega = [0]   # # stores all values of omega at each timestep
t = 0.01
time = [0]
while(t <= 10):
    time.append(t)
    # theta(t+0.01) = theta(t) + 0.01*omega(t)
    theta.append(theta[len(theta)-1] + (0.01)*omega[len(omega)-1])
    # omega(t+0.01) = omega(t) - 0.01*(g/l)*sin(theta(t))
    omega.append(omega[len(omega)-1] - (0.01)*g*np.sin(theta[len(theta)-2])/l)
    t += 0.01


# plotting graphs
plt.plot(timings, theta, label="theta")
plt.plot(timings, omega, label="omega")
plt.xlabel("timesteps")
plt.title("oscillations")
plt.show()

# aniamtion
fig = plt.figure(figsize=(5, 5), facecolor='w')
ax = fig.add_subplot(1, 1, 1)
plt.rcParams['font.size'] = 15

lns = []
for i in range(len(time)):
    ln, = ax.plot([0, np.sin(theta[i])], [0, -np.cos(theta[i])],
                  color='k', lw=2)
    tm = ax.text(-1, 0.9, 'time = %.1fs' % timings[i])
    lns.append([ln, tm])

ax.set_aspect('equal', 'datalim')
ax.grid()
ani = animation.ArtistAnimation(fig, lns, interval=1)
plt.show()
