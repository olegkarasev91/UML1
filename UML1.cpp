﻿#include <iostream>
#include <conio.h>
#include <Windows.h>
#include<chrono>
#include<thread>
using namespace std;
#define MAX_TANK_VOLUME 80
#define MIN_TANK_VOLUME 40
#define MIN_ENGINE_CONSUMPTION 4
#define MAX_ENGINE_CONSUMPTION 25

class Tank // бак
{
	const unsigned int VOLUME; // объем бака
	double fuel_level; // уровень топлива
public:
	const unsigned int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	double fill(double fuel)
	{
		if (fuel_level + fuel < 0)return fuel_level = 0;
		if (fuel_level + fuel > VOLUME)return fuel_level = VOLUME;
		//if (fuel_level+fuel>=0 && fuel_level + fuel <= VOLUME)fuel_level += fuel;
		else return fuel_level += fuel;
	}
	double give_fuel(double fuel)
	{
		fuel_level -= fuel;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	Tank(unsigned int volume) :VOLUME(volume >= MIN_TANK_VOLUME && volume <= MAX_TANK_VOLUME ? volume : MAX_TANK_VOLUME),
		fuel_level(0)
	{
		//this->VOLUME = volume;
		cout << "Tank is ready\t" << this << endl;
	}
	~Tank()
	{
		cout << "Tank is gone\t" << this << endl;
	}
	void info()const
	{
		cout << "Tank volume:\t" << VOLUME << endl;
		cout << "Fuel level:\t" << fuel_level << endl;
	}
};

class Engine // двигатель
{
	double consumption;             //расход
	long double consumption_per_second; // расход в секунду
	bool is_started;
	int speed = 0;
public:
	double get_consumption()const
	{
		return consumption;
	}
	long double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	bool started()const
	{
		return is_started;
	}
	bool start()
	{
		return is_started = true;
	}
	bool stop()
	{
		return is_started = false;
	}
	int get_speed()
	{
		if (started()) {
			char key;
			key = _getch();
			switch (key)
			{
			case 'W': case 'w':
				speed += 1;
				return speed;
				break;
			}
		}
		else speed = 0;
		if (speed > 250) return speed = 250;
		else return speed;
	}
	void set_consumption(double consuption)
	{
		if (consuption >= MIN_ENGINE_CONSUMPTION && consuption <= MAX_ENGINE_CONSUMPTION)
			this->consumption = consuption;
		else
			this->consumption = MAX_ENGINE_CONSUMPTION / 2;
		consumption_per_second = consuption * 3e-4;
		if (speed <= 60)consumption_per_second = 0.020;
		else if(speed >60 && speed <=100)consumption_per_second = 0.014;
		else if(speed > 100 && speed <= 140)consumption_per_second = 0.020;
		else if(speed > 140 && speed <=200)consumption_per_second = 0.025;
		else if(speed > 200 && speed <=250)consumption_per_second = 0.030;
	}
	explicit Engine(double consumption)
	{
		set_consumption(consumption);
		is_started = false;
		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is gone:\t" << this << endl;
	}
	
	void info()const
	{
		cout << "Consumption:\t" << consumption << endl;
		cout << "Consumption per sec:\t" << consumption_per_second << endl;
		cout << "Engine is " << (is_started ? "started" : "stopped") << endl;
		cout << "Speed is " << speed << endl;
	}
};
#define Enter 13
#define Escape 27
class Car
{
	Tank tank;
	Engine engine;
	bool driver_inside;
	struct Control
	{
		std::thread panel_thread; //Отображение панели приборов
		std::thread engine_idle_thread; //холостой ход двигателя
		std::thread free_wheeling_thread; //движение машины по инерции
	}control;
public:
	Car(double engine_consumption, unsigned int tank_volume) :engine(engine_consumption), tank(tank_volume)
	{
		driver_inside = false;
		cout << "Your car is ready to go\t" << this << endl;
	}
	~Car()
	{
		cout << "Your car is over" << endl;
	}
	void fill(double fuel)
	{
		tank.fill(fuel);
	}
	void start_engine()
	{
		if (tank.get_fuel_level())engine.start();
		control.engine_idle_thread = std::thread(&Car::engine_idle, this);
	}
	void stop_engine()
	{
		engine.stop();
		control.engine_idle_thread.join();
	}
	void get_in()
	{
		driver_inside = true;
		control.panel_thread = std::thread(&Car::control_panel, this); //Запускаем control panel в отдельном потоке
		system("CLS");
		cout << "You are out of car" << endl;
	}
	void get_out()
	{
		driver_inside = false;
		control.panel_thread.join();//Останавливаем выполнение потока panel_thread
	}
	void control_car()
	{
		char key;
		do {
			key = _getch();
			switch (key)
			{
			case Enter: //Сесть в машину. Нужно отобразить панел приборов
				if (driver_inside)get_out();
				else get_in();
				break;
			case 'F':case 'f': //Заправить машину
				double fuel;
				cout << "Введите объем топлива: ";cin >> fuel;
				fill(fuel);
				break;
			case 'I': case 'i': //Ignition - зажигание
				if (engine.started())stop_engine();
				else start_engine();
				break;
			case Escape:
				if (engine.started())stop_engine();
				if (control.panel_thread.joinable())get_out();
				break;
			case 'W':
				engine.get_speed();
				break;
			}
		} while (key != 27);
	}
	void engine_idle()
	{
		while (
			engine.started() &&
			tank.give_fuel(engine.get_consumption_per_second())
			)
			std::this_thread::sleep_for(1s);
	}
	void control_panel()
	{
		while (driver_inside)
		{
			system("CLS");
			cout << "Fuel level: " << tank.get_fuel_level() << " liters.\n";
			cout << "Engine is  " << (engine.started() ? "started" : "stopped") << endl;
			cout << "Speed is: " << engine.get_speed() << endl;
			cout << "Consumption is: " << engine.get_consumption_per_second() << endl;
			std::this_thread::sleep_for(1ms);
		}
	}
	void info()const
	{
		tank.info();
		engine.info();
	}

};

//#define TANK_CHECK
//#define ENGINE_CHECK
void main()
{
	setlocale(LC_ALL, "");
#ifdef TANK_CHECK
	Tank tank(120);
	tank.info();
	int fuel;
	while (true)
	{
		cout << "Введите объем: ";cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
#endif//TANK CHECK
#ifdef ENGINE_CHECK
	//Engine engine(9);
	//engine.info();
#endif//ENGINE_CHECK

	Car bmw(8, 80);
	cout << "Press Enter to get in" << endl;
	bmw.control_car();
}