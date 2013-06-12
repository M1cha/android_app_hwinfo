package com.xiaomi.hwinfo;

import java.util.ArrayList;
import com.stericson.RootTools.RootTools;
import com.stericson.RootTools.execution.Command;
import com.stericson.RootTools.execution.CommandCapture;
import com.xiaomi.hwinfo.R;
import android.os.Bundle;
import android.app.Activity;
import android.view.KeyEvent;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends Activity {

	static {
		System.loadLibrary("hwinfo");
	}
	private native void getHwInfo(HWInfo hwinfo);
	private native void getABootInfo(ABootDeviceInfo abootinfo);
	
	private void gainAbootAccess() {
		CommandCapture command = new CommandCapture(0,
				"busybox chmod 0777 /dev/block/platform/msm_sdcc.1/by-name/aboot");

		try {
			RootTools.getShell(true).add(command).waitForFinish();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private String fileGetContents(String file) {
		final StringBuilder sb = new StringBuilder();
		
		Command command = new Command(0, "busybox cat \""+file+"\"")
		{
		        @Override
		        public void output(int id, String line)
		        {
		            sb.append(line);
		        }
		};
		
		try {
			RootTools.getShell(false).add(command).waitForFinish();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return sb.toString();
	}
	
	private String getInputDevices() {
		final ArrayList<Device> devices = new ArrayList<Device>();
		
		Command command = new Command(0, "busybox find /sys/devices/i2c*/*/*/ -name name")
		{
		        @Override
		        public void output(int id, String line)
		        {
		        	String[] parts = line.split("/");
		        	if(parts[4].equals("i2c-dev")) return;
		        	Device dev = new Device();
		        	dev.bus = parts[3];
		        	dev.device = parts[4];
		        	dev.type = parts[5];
		        	dev.line = line;
		        	devices.add(dev);
		        }
		};
		
		try {
			RootTools.getShell(false).add(command).waitForFinish();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		// get names
		for(Device dev : devices) {
			dev.name = fileGetContents(dev.line);
		}
		
		String ret = "";
		for(Device dev : devices) {
			ret+=dev.toString();
		}
		
		return ret;
	}

	private TextView labelText;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_main);
		labelText = (TextView)findViewById(R.id.text);
		
		gainAbootAccess();
		updateUI();
	}

	private void updateUI() {
		HWInfo hwinfo = new HWInfo();
		getHwInfo(hwinfo);
		
		ABootDeviceInfo abootinfo = new ABootDeviceInfo();
		getABootInfo(abootinfo);
		
		StringBuilder sb = new StringBuilder();
		sb.append(hwinfo.toString()+"\n");
		sb.append(getInputDevices()+"\n");
		sb.append("SystemType: "+fileGetContents("/sys/devices/system/soc/soc0/hw_platform")+"\n");
		sb.append("\nAbootInfo:\n");
		sb.append("magic: "+abootinfo.magic+"\n");
		sb.append("is_tampered: "+abootinfo.is_tampered+"\n");
		sb.append("is_unlocked: "+abootinfo.is_unlocked+"\n");
		
		labelText.setText(sb);
	}

	public void onClickHandler(View v) {
		switch (v.getId()) {
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			finish();
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

}
