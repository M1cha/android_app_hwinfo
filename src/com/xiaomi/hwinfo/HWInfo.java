package com.xiaomi.hwinfo;

public class HWInfo {
	public int touch_info;
	public int soc_info;
	public int ddr_info;
	public int emmc_info;
	public int cpu_info;
	public int pmic_info;
	public int panel_info;
	public int reserved_bit;
	
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		
		sb.append("touch_info: "+touch_info+"\n");
		sb.append("soc_info: "+soc_info+"\n");
		sb.append("ddr_info: "+ddr_info+"\n");
		sb.append("emmc_info: "+emmc_info+"\n");
		sb.append("cpu_info: "+cpu_info+"\n");
		sb.append("pmic_info: "+pmic_info+"\n");
		sb.append("panel_info: "+panel_info+"\n");
		sb.append("reserved_bit: "+reserved_bit+"\n");
		
		return sb.toString();
	}
}
