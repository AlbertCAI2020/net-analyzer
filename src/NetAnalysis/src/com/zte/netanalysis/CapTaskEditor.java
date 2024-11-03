package com.zte.netanalysis;

import com.zte.netanalysis.capinfo.CapInfo;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.net.Uri;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class CapTaskEditor {

	static class Result{
		public String url;
		public long timeout;
		public int repeat;
		public String usrName;
		public String usrPwd;
	}	
	
	public interface OnResult{
		public void onResult(int actionType, Result result);
	}
	
	private OnResult resultCallback;
	private AlertDialog dialog;
	private int actionType;
	private Context context;
	
	public CapTaskEditor(Context context, int actionType, OnResult resultCallback){
		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		View dlgView = buildDialogView(context, actionType);
		builder.setView(dlgView);
		builder.setTitle("Task Editor");
		builder.setPositiveButton("OK", new OnDlgOkListener());
		builder.setNegativeButton("Cancel", null);
		dialog = builder.create();
		
		this.actionType = actionType;
		this.resultCallback = resultCallback;
		this.context = context;
	}
	
	public void show(){
		dialog.show();
	}
	
	private View buildDialogView(Context context, int type){
		View view = null;
		if(CapInfo.WEBPAGE_BROWSE == type){
			view = View.inflate(context, R.layout.dialog_browsetaskedit, null);
		}else if(CapInfo.FILE_DOWNLOAD == type){
			view = View.inflate(context, R.layout.dialog_downloadtaskedit, null);
		}else if(CapInfo.MEDIA_PLAY == type){
			view = View.inflate(context, R.layout.dialog_mediaplaytaskedit, null);
		}
		return view;
	}
	
	private class OnDlgOkListener implements DialogInterface.OnClickListener{
		@Override
		public void onClick(DialogInterface dialog, int which) {
			AlertDialog dlg = (AlertDialog)dialog;
			EditText urlEdit = (EditText)dlg.findViewById(R.id.urlEdit);
			EditText maxTimeEdit = (EditText)dlg.findViewById(R.id.maxTimeEdit);
			EditText repeatEdit = (EditText)dlg.findViewById(R.id.repeatEdit);
			
			String strUrl = urlEdit.getText().toString();
			Uri url = Uri.parse(strUrl);
			String host = url.getHost();
			if(null == host || host.isEmpty()){
				Toast.makeText(context, "wrong url!", Toast.LENGTH_SHORT).show();
				return;
			}
			
			long timeout = Long.parseLong(maxTimeEdit.getText().toString());
			int repeat = Integer.parseInt(repeatEdit.getText().toString());
			
			Result result = new Result();
			result.url = strUrl;
			result.timeout = timeout*1000;
			result.repeat = repeat;
			
			if(CapInfo.FILE_DOWNLOAD == actionType){
				EditText usrNameEdit = (EditText)dlg.findViewById(R.id.usrNameEdit);
				EditText passwordEdit = (EditText)dlg.findViewById(R.id.passwordEdit);
				result.usrName = usrNameEdit.getText().toString();
				result.usrPwd = passwordEdit.getText().toString();
			}
			if(null != resultCallback){
				resultCallback.onResult(actionType, result);
			}
		}
		
	}
	
}
