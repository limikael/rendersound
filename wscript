def options(opt):
	opt.load("compiler_c")
	opt.add_option("--debug",action="store_true",default=False,help="Debug traces")

def configure(cnf):
	cnf.load("compiler_c")
	cnf.env.append_value("CFLAGS",["-Wall"])

	if cnf.options.debug:
		cnf.env.append_value("CFLAGS",["-DDEBUG"])

	cnf.check(features='c cprogram',lib="gd",uselib_store="GD")
	cnf.check_cfg(package="libavcodec",uselib_store="AVCODEC",args='--cflags --libs')
	cnf.check_cfg(package="libavformat",uselib_store="AVFORMAT",args='--cflags --libs')

def build(bld):
	bld(
		features="c cprogram", 
		source=['src/rendersound.c','src/soundrenderer.c','extern/ffsnd/src/ffsndin.c','extern/ffsnd/src/ffsndutil.c'], 
		target='rendersound', 
		use=['GD','AVCODEC','AVFORMAT'],
		includes='extern/ffsnd/src'
	)