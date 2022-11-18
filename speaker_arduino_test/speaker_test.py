import serial
import time
import scipy.io.wavfile as wavfile
import numpy as np
import math

#np.set_printoptions(threshold=np.inf)

arduino = serial.Serial(port='COM11', baudrate=115200, timeout=.1)
samplerate, audio_file = wavfile.read('fur_elise_8bit_8khz.wav')
total_audio_time = len(audio_file)/samplerate
loop_time = 1/samplerate

for i,sample in enumerate(audio_file):
    loop_start_time = time.time()
    arduino.write(bytes(str(sample), 'utf-8'))
    print(total_audio_time*i/len(audio_file), "seconds") # printing the value
    loop_time_taken = time.time() - loop_start_time

    #time.sleep((loop_time-loop_time_taken)*(math.copysign(1,loop_time-loop_time_taken)+1)/2)