package com.zte.netanalysis;

import java.util.ArrayList;
import java.util.List;

import com.zte.netanalysis.capinfo.CapInfoTCP;

import android.content.Context;
import android.util.AttributeSet;
import lecho.lib.hellocharts.gesture.ContainerScrollType;
import lecho.lib.hellocharts.model.Axis;
import lecho.lib.hellocharts.model.Line;
import lecho.lib.hellocharts.model.LineChartData;
import lecho.lib.hellocharts.model.PointValue;
import lecho.lib.hellocharts.model.ValueShape;
import lecho.lib.hellocharts.model.Viewport;
import lecho.lib.hellocharts.util.ChartUtils;
import lecho.lib.hellocharts.view.LineChartView;


public class TcpSpeedChartView extends LineChartView{
	private List<PointValue> ptValues;
	private int realPointNum;
	private Line line ;
	private List<Line> lines;
	private LineChartData chartData;
	private float maxSpeed;
	private float maxTime;
	private CapInfoTCP.IntervalStats data;
	
	public TcpSpeedChartView(Context context) {
		this(context, null, 0);
	}
	
	public TcpSpeedChartView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

	
    public TcpSpeedChartView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        
        ptValues = new ArrayList<PointValue>();		
		line = new Line(ptValues);
		line.setColor(ChartUtils.COLOR_BLUE);
		line.setShape(ValueShape.CIRCLE);
		line.setCubic(false);
		line.setHasPoints(true);
		line.setStrokeWidth(1);
		line.setPointRadius(2);		
		lines = new ArrayList<Line>();
		lines.add(line);
		
		chartData = new LineChartData(lines);		
		Axis axisX = new Axis();
		Axis axisY = new Axis().setHasLines(true);      
		axisX.setName("time(s)");
		axisY.setName("speed(KB/s)");         
		chartData.setAxisXBottom(axisX);
		chartData.setAxisYLeft(axisY);
		
		setLineChartData(chartData);
		setViewportCalculationEnabled(false);
		
		Viewport cv = getCurrentViewport();
		cv.set(0, 20, 10, 0);
		setCurrentViewport(cv);		
		Viewport mv = getMaximumViewport();		
		mv.set(0, 20, 10, 0);
		setMaximumViewport(mv);       
    }
	
	public void bindData(CapInfoTCP.IntervalStats data){
		
		ptValues.clear();
		realPointNum = 0;
		maxSpeed = 0;
		maxTime = 0;		
		Viewport cv = getCurrentViewport();
		cv.set(0, 20, 10, 0);
		setCurrentViewport(cv);		
		Viewport mv = getMaximumViewport();		
		mv.set(0, 20, 10, 0);
		setMaximumViewport(mv);
		
		this.data = data;
		refresh();
	}
	
	public void refresh(){
		reCalculateLinePoints();
		reCalculateViewPort();
		super.onChartDataChange();
	}
	
	private void reCalculateViewPort(){
		Viewport cv = getCurrentViewport();
		Viewport mv = getMaximumViewport(); 
		
		boolean setMax = false;
		if(mv.top < maxSpeed){
			mv.top = maxSpeed + maxSpeed*0.1f;
			setMax = true;
		}
		if(mv.right < maxTime){
			mv.right = maxTime + 10.0f;
			setMax = true;
		}
		if(setMax){
			setMaximumViewport(mv);	
		}
		
		boolean setCur = false;
		if(cv.top < maxSpeed){
			cv.top = maxSpeed + 5.0f;
			setCur = true;
		}
		
		if(cv.right < maxTime + 2.0f){
			float w = cv.width();
			cv.right = maxTime + 1.0f;
			cv.left = cv.right - w;
			setCur = true;
		}
		if(setCur){
			setCurrentViewport(cv);
		}
	}
	
	private void reCalculateLinePoints(){
		if(null == data){
			return;
		}
		List<CapInfoTCP.InterStat> dataList = data.getIntervalStats();
		int dataNum = dataList.size();
		if(realPointNum >= dataNum){
			return;
		}
		CapInfoTCP.InterStat fst = dataList.get(0);
		long start = fst.beginTime;
		long last = start;
		long interval = fst.endTime - fst.beginTime;
		if(realPointNum > 0){
			last = dataList.get(realPointNum-1).endTime;
		}
		
		for(int i = realPointNum; i < dataNum; i++){
			CapInfoTCP.InterStat stat = dataList.get(i);
			long current = stat.endTime;
			
			float speed = (float)stat.effectivSpeed();			
			float time = (float)(current-start)/1000.0f;
			if(speed > maxSpeed){
				maxSpeed = speed;
			}
			if(time > maxTime){
				maxTime = time;
			}
			
			long count = (current - last)/interval;			
			if(count == 1){
				ptValues.add(new PointValue(time, speed));
				realPointNum++;
			}else if(count == 2){
				float fillTime = (float)(current - interval - start)/1000.0f;
				ptValues.add(new PointValue(fillTime, 0.0f));
				ptValues.add(new PointValue(time, speed));
				realPointNum++;
			}else if(count > 2){
				float fillTime1 = (float)(last + interval - start)/1000.0f;
				float fillTime2 = (float)(current - interval - start)/1000.0f;
				ptValues.add(new PointValue(fillTime1, 0.0f));
				ptValues.add(new PointValue(fillTime2, 0.0f));
				ptValues.add(new PointValue(time, speed));
				realPointNum++;
			}
			
			last = current;
		}	
		
	}
}
