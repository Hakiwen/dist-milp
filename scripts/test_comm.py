import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)

Left_Comm_Path_status = 7
Right_Comm_Path_status = 11
Left_Neighbor_status = 12
Right_Neighbor_status = 16

GPIO.setup(Left_Comm_Path_status, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(Right_Comm_Path_status, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(Left_Neighbor_status, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(Right_Neighbor_status, GPIO.IN, pull_up_down = GPIO.PUD_UP)

while True:
	print("Left_Comm_Path_status: ", GPIO.input(Left_Comm_Path_status))
	print("Right_Comm_Path_status: ", GPIO.input(Right_Comm_Path_status))
	print("Left_Neighbor_status: ", GPIO.input(Left_Neighbor_status))
	print("Right_Neighbor_status: ", GPIO.input(Right_Neighbor_status))
	print("")
	time.sleep(1)