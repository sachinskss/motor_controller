import sys
import pandas as pd
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_logs.py <csv_file>")
        return
    df = pd.read_csv(sys.argv[1])
    plt.figure()
    plt.subplot(2,1,1)
    plt.plot(df['time'], df['speed'], label='Speed (rad/s)')
    plt.ylabel('Speed')
    plt.legend()
    plt.subplot(2,1,2)
    plt.plot(df['time'], df['vd'], label='Vd')
    plt.plot(df['time'], df['vq'], label='Vq')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage')
    plt.legend()
    plt.show()

if __name__ == '__main__':
    main()