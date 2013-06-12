package com.xiaomi.hwinfo;

public class Device {
	public String bus;
	public String device;
	public String type;
	public String name;
	public String line;
	
	@Override
	public String toString() {
		return "Bus: "+bus+" ; Device: "+device+" ; Type: "+type+" ; Name: "+name+"\n";
	}
}
