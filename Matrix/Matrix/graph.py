import matplotlib.pyplot as plt
import numpy as np
x = np.linspace(0, 10, 10)
y = np.linspace(0, 10, 10)
matrix_size = np.arange(64, 1152+1, 64);
number_of_threads = np.arange(1, 32+1, 1)
mono_time_s = np.array([0.0008816, 0.0070503, 0.0228255, 0.0536088, 0.123613, 0.245435, 0.468917, 0.544583, 0.597573, 0.821917, 1.10375, 1.4597, 1.81059, 2.27691, 2.79402, 3.44132, 4.62181, 5.50022])
multi_time_s = np.array([0.0087562, 0.0076891, 0.0205777, 0.0221137, 0.0326198, 0.0519611, 0.0864848, 0.119237, 0.174549, 0.239947, 0.314836, 0.413064, 0.550976, 0.678116, 0.833463, 1.00779, 1.19833, 1.4025])
multi_time_th = np.array([4.75357, 2.6205, 1.98829, 1.63881, 1.53079, 1.49393, 1.43609, 1.39041, 1.41884, 1.46931, 1.39514, 1.47386, 1.60525, 1.41947, 1.42174, 1.52673, 1.44878, 1.52287, 1.66983, 1.56387, 1.61066, 1.57515, 1.52863, 1.56232, 1.45016, 1.4678, 1.49617, 1.55573, 1.53496, 1.52675, 1.51045, 1.50205])
time1152 = np.max(mono_time_s)
g_times = time1152/multi_time_th
# print(len(multi_time_th), " ", len(number_of_threads))
# print(len(mono_time_s), " ", len(matrix_size))
# print(number_of_threads)
print(time1152)
# print(matrix_size)

fig, ax = plt.subplots()
ax.scatter(matrix_size, mono_time_s)
ax.scatter(matrix_size, multi_time_s)
ax.set_title("Multi and Mono from matrix size")
ax.set_xlabel("Matrix size")
ax.set_ylabel("Time of execution")
ax.grid()

fig, ax = plt.subplots()
ax.scatter(number_of_threads, multi_time_th)
ax.set_title("Multi from number of threads")
ax.set_xlabel("Number of threads")
ax.set_ylabel("Time of execution")
ax.grid()

fig, ax = plt.subplots()
ax.scatter(number_of_threads, g_times)
ax.grid()
ax.set_title("Multi/Mono on fixed size")
ax.set_xlabel("number of threads")
ax.set_ylabel("Multi/Mono")
plt.show()