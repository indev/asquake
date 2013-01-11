package se.quake
{
	import cmodule.swc.quake.CLibInit;
	
	import flash.display.Bitmap;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.geom.Point;
	import flash.net.SharedObject;
	import flash.net.URLRequest;
	import flash.system.System;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Keyboard;
	import flash.ui.Mouse;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	import flash.utils.getTimer;
	
	public class FlashQuake extends Sprite
	{
		public static var QUAKE_READY:String	= "quakeReady";
		
		private var _cLoader:CLibInit;
		private var _cLib:Object;
		
		private var _filenames:Array;
		private var _bytearrays:Object;
				
		private var _videoBuffer:VideoBuffer;
		private var _soundBuffer:SoundBuffer;
		
		private var _status:TextField;
		
		private var _soundfilenames:Array;
		private var _quakesounds:Array;
		
		private var _lastTick:int;
		
		private var _isMouseDown:Boolean;
		
		private var _oldmouse:Point;
		private var _mouse:Point;
		private var _totalfiles:uint;
		
		private var _memoryTimer:Timer;
		
		public function FlashQuake()
		{
			super();
			
			_bytearrays = {};
			_quakesounds = [];
			
			// data files list
			_filenames = SupplyFile.quakefiles;
			_soundfilenames = SupplyFile.quakesoundfiles
			
			_filenames = _filenames.concat( _soundfilenames );
			_totalfiles = _filenames.length;
			/*
			var filename:String;
			for ( var i:int = 0; i < _filenames.length; ++i )
			{
				filename = _filenames[i];
				trace('[Embed(source="../../../bin-debug/'+filename+'", mimeType="application/octet-stream")]');
				filename = filename.split("/").join("_");
				filename = filename.split(".").join("_");
				trace('private var '+filename+':Class;');
				trace('');
			}
			*/
			//_embeddedFiles = new EmbeddedFiles();
			
			// create status textfield 
			_status = new TextField();
			var tfm:TextFormat = new TextFormat();
			tfm.size = 11;
			tfm.font = "Arial";
			tfm.color = 0x000000;
			_status.height = 16;
			_status.width = VideoBuffer.QUAKE_WIDTH;
			_status.background = true;
			_status.backgroundColor = 0xFFFFFF;
			_status.setTextFormat( tfm );
			
			_status.x = 0;
			_status.y = VideoBuffer.QUAKE_HEIGHT - _status.height;
			
			_isMouseDown = false;
			_mouse = new Point(0,0);
			_oldmouse = new Point(0,0);
			
			addChild(_status);
			
			// init C library
			_cLoader = new CLibInit();
			
			// supply the shared object files
			var so:SharedObject = SharedObject.getLocal("sharedfiles");
			if ( so.data.files ) {
				for ( var i:int = 0; i < so.data.files.length; ++i ) {
					var so2:SharedObject = SharedObject.getLocal(so.data.files[i]);
					if ( so2.data.byteArray )
						_cLoader.supplyFile( so.data.files[i], so2.data.byteArray );
				}
			}
			
			_memoryTimer = new Timer(500,1);
			_memoryTimer.addEventListener(TimerEvent.TIMER_COMPLETE, onMemoryTimer );
			
			// start loading
			loadNextSupplyFile();
		}
		
		private function setStatus( str:String ):void
		{
			_status.text = str;
		}
		
		private function onSupplyFileFailed(event:IOErrorEvent):void
		{
			var supplyfile:SupplyFile = SupplyFile(event.target);
			supplyfile.removeEventListener(Event.COMPLETE, onSupplyFileComplete);
			supplyfile.removeEventListener(IOErrorEvent.IO_ERROR, onSupplyFileFailed);
				
			trace("supplyFile (failed): " + supplyfile.cFilename);
			
			loadNextSupplyFile();
		}
		
		private function onSupplyFileComplete(event:Event):void
		{
			var supplyfile:SupplyFile = SupplyFile(event.target);
			supplyfile.removeEventListener(Event.COMPLETE, onSupplyFileComplete);
			supplyfile.removeEventListener(IOErrorEvent.IO_ERROR, onSupplyFileFailed);
			
			var bytes:ByteArray = new ByteArray();
			supplyfile.readBytes( bytes );
			
			//_bytearrays[supplyfile.cFilename] = bytes;
			
			_cLoader.supplyFile( supplyfile.cFilename, bytes );
			
			trace("supplyFile: " + supplyfile.cFilename);
			
			loadNextSupplyFile();
		}		
		
		private function loadNextSupplyFile():void
		{
			var filename:String = _filenames.pop();
			
			if ( filename )
			{
				var supplyfile:SupplyFile = new SupplyFile();
				supplyfile.cFilename = filename;
				supplyfile.addEventListener(Event.COMPLETE, onSupplyFileComplete);
				supplyfile.addEventListener(IOErrorEvent.IO_ERROR, onSupplyFileFailed);
				
				setStatus("Loading: " + filename + " ("+(_totalfiles-_filenames.length)+"/"+_totalfiles+")");

				supplyfile.load( new URLRequest(filename) );
				//var bytes:ByteArray = _embeddedFiles.getFile(filename);
				//_bytearrays[filename] = bytes;
				//_cLoader.supplyFile( filename, bytes );
			}
			else
			{
				_memoryTimer.start();
				//loadNextSoundFile();
			}
		}
		
		private function onMemoryTimer(event:Event):void
		{
			// init the C lib
			_cLib = _cLoader.init();
				
			_memoryTimer.stop();
			_memoryTimer.removeEventListener(TimerEvent.TIMER_COMPLETE, onMemoryTimer );
			
			runQuake();
		}
		
		private function runQuake():void
		{
//			try 
			{
				setStatus("init quake");
				
				// create the flash video buffer
				_videoBuffer = new VideoBuffer();
				addChild(_videoBuffer);
				
				// create the sound buffer
				_soundBuffer = new SoundBuffer(_cLib);
				//addChild( _soundBuffer );
		
				// register this object to the C engine, allows the quake C to call methods here
				//_cLib.quakeRegisterFlashQuake( this );
					
				// exposed method, runs Host_Init();
				_cLib.quakeInit();
				
				// add input listeners
				this.addEventListener(KeyboardEvent.KEY_UP, keyUp);
				this.addEventListener(KeyboardEvent.KEY_DOWN, keyDown);
				this.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
				this.addEventListener(MouseEvent.MOUSE_UP, mouseUp);
				stage.addEventListener(Event.MOUSE_LEAVE, mouseLeaveStage);
				
				dispatchEvent( new Event(QUAKE_READY) );
				
				// run the fram ticker
				setStatus("run quake");
				_lastTick = getTimer();
				addEventListener(Event.ENTER_FRAME, enterFrame);
			}
//			catch (e:Error) 
			{
//				trace("Looks like we ran out of memory: " + e.message);
			}
		}
		
		private function enterFrame(event:Event):void
		{
			stage.focus = this;
			this.focusRect = false;
			
			// set the mouse position
			if ( _isMouseDown )
			{
				_mouse.x = this.mouseX;
				_mouse.y = this.mouseY;
				
				var dx:int = (_mouse.x - _oldmouse.x) * 3;
				var dy:int = (_mouse.y - _oldmouse.y) * 3;
				
				_cLib.quakeSetMousePosition( dx, dy );
				
				_oldmouse.x = _mouse.x;
				_oldmouse.y = _mouse.y; 
			}
			
			// exposed method, runs Host_Frame();
			var tick:Number = getTimer() - _lastTick;
			_cLib.quakeTick( tick / 1000 );
			_lastTick = getTimer();
			
			// get the video frame memory pointer and update screen
			_videoBuffer.updateVideoBufferPtr( _cLib.quakeGetVideoBufferPtr() );
			
			// update the sound buffer
			_soundBuffer.updateSoundBuffer(); 
		}
		
		private function mouseLeaveStage(event:Event):void
		{
			Mouse.show();
			_isMouseDown = false;
		}
		
		private function mouseDown(event:MouseEvent):void
		{
			Mouse.hide();
			_isMouseDown = true;
			_mouse.x = _oldmouse.x = this.mouseX;
			_mouse.y = _oldmouse.y = this.mouseY;
		}
		
		private function mouseUp(event:MouseEvent):void
		{
			Mouse.show();
			_isMouseDown = false;
		}

		private function keyUp(event:KeyboardEvent):void
		{
			_cLib.quakeSetKeys( convertKeyCode(event.keyCode, event.charCode), 0 );
		}
		
		private function keyDown(event:KeyboardEvent):void
		{
			_cLib.quakeSetKeys( convertKeyCode(event.keyCode, event.charCode), 1 );
		}
		
		private function convertKeyCode( keyCode:uint, charCode:uint ):uint
		{
			/*
				#define	K_BACKSPACE		127
				#define	K_UPARROW		128
				#define	K_DOWNARROW		129
				#define	K_LEFTARROW		130
				#define	K_RIGHTARROW	131
				#define	K_ALT			132
				#define	K_CTRL			133
				#define	K_SHIFT			134
				#define	K_INS			147
				#define	K_DEL			148
				#define	K_PGDN			149
				#define	K_PGUP			150
				#define	K_HOME			151
				#define	K_END			152
			*/
			switch (keyCode)
			{
				case Keyboard.BACKSPACE: return 127;
				case Keyboard.UP: return 128;
				case Keyboard.DOWN: return 129;
				case Keyboard.LEFT: return 130;
				case Keyboard.RIGHT: return 131;
				case Keyboard.CONTROL: return 133;
				case Keyboard.SHIFT: return 134;
				case Keyboard.PAGE_DOWN: return 149;
				case Keyboard.PAGE_UP: return 150;
				case Keyboard.HOME: return 151;
				case Keyboard.END: return 152;
				case Keyboard.DELETE: return 148;
				default: return charCode;
			}
		}
		
		// Writing and reading files is based on Michael Rennie's Quake port 
		public function fileWriteSharedObject(filename:String):ByteArray
		{
			var sharedObject:SharedObject = SharedObject.getLocal(filename);
			if (!sharedObject)
				return undefined;	//Shared objects not enabled
			
			var byteArray:ByteArray;
			if (!_bytearrays[filename])
			{
				//Havent yet created a ByteArray for this file, so create a blank one.
				byteArray = new ByteArray;
				_bytearrays[filename] = byteArray;
				
				//Supply the ByteArray as a file, so that it can also be read later on, if needed.
				_cLoader.supplyFile(filename, byteArray);
				
				var so:SharedObject = SharedObject.getLocal("sharedfiles");
				if( !so.data.files )
					so.data.files = new Array();
				
				so.data.files.push(filename);
			}
			else
			{
				byteArray = _bytearrays[filename];
				
				//We are opening the file for writing, so reset its length to 0.
				//Needed because this is NOT done by funopen(byteArray, ...)
				byteArray.length = 0;
			}
			
			//Return the ByteArray, allowing it to be opened as a FILE* for writing using funopen(byteArray, ...)
			return byteArray;
		}
		
		//SharedObjects are used to save quake config files	
		public function fileUpdateSharedObject(filename:String):void
		{
			var sharedObject:SharedObject = SharedObject.getLocal(filename);
			
			if (!sharedObject)
				return;			//Shared objects not enabled
				
			if (!_bytearrays[filename])
			{
				//This can happen if fileUpdateSharedObject is called before fileWriteSharedObject or fileReadSharedObject
				trace("Error: fileUpdateSharedObject() called on a file without a ByteArray");
			}
			
			sharedObject.data.byteArray = _bytearrays[filename];
			sharedObject.flush();
		}
	}
}