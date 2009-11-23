package se.quake
{
	import cmodule.swc.quake.CLibInit;
	
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.KeyboardEvent;
	import flash.events.TimerEvent;
	import flash.net.URLRequest;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Keyboard;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	
	public class FlashQuake extends Sprite
	{
		private var _cLoader:CLibInit;
		private var _cLib:Object;
		
		private var _filenames:Array;
		private var _bytearrays:Array;
		
		private var _ticker:Timer;
		
		private var _videoBuffer:VideoBuffer;
		
		private var _status:TextField;
		
		private var _soundfilenames:Array;
		private var _quakesounds:Array;
		
		public function FlashQuake()
		{
			super();
			
			_bytearrays = [];
			_quakesounds = [];
			
			// data files list
			_filenames = SupplyFile.quakefiles;
			_soundfilenames = SupplyFile.quakesoundfiles
			
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
			
			addChild(_status);
			
			// init the frame ticker
			_ticker = new Timer(40,0);
			_ticker.addEventListener(TimerEvent.TIMER, onTick);
			
			// init C library
			_cLoader = new CLibInit();
			
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
			
			_bytearrays.push( bytes );
			
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
				
				setStatus("Loading: " + filename);

				supplyfile.load( new URLRequest(filename) );
			}
			else
			{
				runQuake();
				//loadNextSoundFile();
			}
		}
		
		private function onQuakeSoundFailed(event:IOErrorEvent):void
		{
			var quakesound:QuakeSound = QuakeSound(event.target);
			quakesound.removeEventListener(Event.COMPLETE, onQuakeSoundComplete);
			quakesound.removeEventListener(IOErrorEvent.IO_ERROR, onQuakeSoundFailed);
				
			trace("loadSound (failed): " + quakesound.soundname);
			
			loadNextSoundFile();
		}
		
		private function onQuakeSoundComplete(event:Event):void
		{
			var quakesound:QuakeSound = QuakeSound(event.target);
			quakesound.removeEventListener(Event.COMPLETE, onQuakeSoundComplete);
			quakesound.removeEventListener(IOErrorEvent.IO_ERROR, onQuakeSoundFailed);
			
			_quakesounds.push( quakesound );
			
			trace("loadSound: " + quakesound.soundname);
			
			loadNextSoundFile();
		}
		
		private function loadNextSoundFile():void
		{
			var filename:String = _soundfilenames.pop();
			
			if ( filename )
			{
				var quakeSound:QuakeSound = new QuakeSound();
				quakeSound.soundname = filename;
				quakeSound.addEventListener(Event.COMPLETE, onQuakeSoundComplete);
				quakeSound.addEventListener(IOErrorEvent.IO_ERROR, onQuakeSoundFailed);
				
				setStatus("Loading sound: " + filename);

				quakeSound.load( new URLRequest(filename) );
			}
			else
			{
				runQuake();
			}
		}
		
		public function playQuakeSound( soundname:String ):void
		{
			trace("playQuakeSound");
			var sound:QuakeSound = findQuakeSoundFile(soundname);
		}
		
		public function testFunc():String
		{
			trace("testFunc");
			return "";
		}
		
		private function findQuakeSoundFile( soundname:String ):QuakeSound
		{
			var i:int;
			for ( ; i < _quakesounds.length; ++i )
				if ( _quakesounds[i].soundname == soundname )
					return _quakesounds[i];
			
			return null;
		}
		
		private function runQuake():void
		{
			setStatus("init quake");
			
			// create the flash video buffer
			_videoBuffer = new VideoBuffer();
			addChild(_videoBuffer);
			
			// init the C lib
			_cLib = _cLoader.init();
			
			// register this object to the C engine, allows the quake C to call methods here
			_cLib.quakeRegisterFlashQuake( this );

			// exposed method, runs Host_Init();
			_cLib.quakeInit();
			
			// palette should be ready by now
			_videoBuffer.setPalette( _cLib.quakeGetPalette() );
			
			// add input listeners
			this.addEventListener(KeyboardEvent.KEY_UP, keyUp);
			this.addEventListener(KeyboardEvent.KEY_DOWN, keyDown);
			
			// run the fram ticker
			setStatus("run quake");
			_ticker.start();
		}
		
		private function onTick(event:TimerEvent):void
		{
			stage.focus = this;
			this.focusRect = false;
			
			// exposed method, runs Host_Frame();
			_cLib.quakeTick();
			
			_videoBuffer.setPalette( _cLib.quakeGetPalette() );
			
			// get the video frame memory pointer and update screen
			_videoBuffer.updateVideoBufferPtr( _cLib.quakeGetVideoBuffer() );
		}
		
		private function keyUp(event:KeyboardEvent):void
		{
			_cLib.quakeSetKeys( convertKeyCode(event.keyCode), 0 );
		}
		
		private function keyDown(event:KeyboardEvent):void
		{
			_cLib.quakeSetKeys( convertKeyCode(event.keyCode), 1 );
		}
		
		private function convertKeyCode( keyCode:uint ):uint
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
				case Keyboard.PAGE_DOWN: return 149;
				case Keyboard.PAGE_UP: return 150;
				case Keyboard.SHIFT: return 134;
				case Keyboard.CONTROL: return 133;
				case Keyboard.DELETE: return 148;
				case Keyboard.INSERT: return 147;
				case Keyboard.HOME: return 151;
				case Keyboard.END: return 152;
				
				default: return keyCode;
			}
		}
	}
}