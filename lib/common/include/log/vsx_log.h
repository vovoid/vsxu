#ifndef VSX_LOG_H_
#define VSX_LOG_H_

// logging
//extern int engine_log_level;
//extern vsx_string<>engine_log_dir;
void log(vsx_string<> message, int level = 1);


#define LOG(mess)
//log(mess);
#define LOG2(mess, lvl)
//log(mess, lvl);

#define LOG3(mess) log(mess);



#endif /*VSX_LOG_H_*/
