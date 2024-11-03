
import sqlite3
import time

db = sqlite3.connect('./capinfo.db')
cu = db.cursor()
cu.execute("SELECT capinfo.startTime, capinfo.url, dlflowinfo.totalBytes, "
	"dlflowinfo.retransAckedBytes, dlflowinfo.firstPayloadTime, dlflowinfo.lastPayloadTime "
	"FROM capinfo LEFT JOIN dlflowinfo ON capinfo.startTime=dlflowinfo.parentId AND dlflowinfo.sessionId=0")
	
out = open('result.txt', 'wt')
for item in cu.fetchall():
	timestr = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(item[0]/1000))
	url = item[1]
	speed = (item[2]-item[3])*1.0/(item[5]-item[4])
	out.write('%s,  %s,  %.3fKB/s\n'%(timestr,url,speed))
out.close()
