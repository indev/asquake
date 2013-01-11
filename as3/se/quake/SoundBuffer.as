package se.quake
{
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.SampleDataEvent;
	import flash.media.Sound;
	import flash.media.SoundChannel;
	
	public class SoundBuffer extends Sprite
	{
		private var _sound:Sound;
		private var _soundChannel:SoundChannel; 
		private var _lastSampleDataPosition:uint;
		
		private var _cLib:Object;
		
		
		public function SoundBuffer( cLib:Object )
		{
			_cLib = cLib;
			_lastSampleDataPosition = 0;
			
			_sound = new Sound();
			_sound.addEventListener(SampleDataEvent.SAMPLE_DATA, onSampleData);
			
			super();
		}
		
		public function updateSoundBuffer():void
		{
			if ( !_soundChannel )
			{
				 _lastSampleDataPosition = 0;
				_soundChannel = _sound.play();
				_soundChannel.addEventListener(Event.SOUND_COMPLETE, soundComplete);
			}
		}
		
		private function soundComplete(event:Event):void
		{
			_soundChannel.removeEventListener(Event.SOUND_COMPLETE, soundComplete);
			_soundChannel = null;
		}

		private function onSampleData(event:SampleDataEvent):void 
		{
			var tick:int = event.position - _lastSampleDataPosition;
			_cLib.quakePaintSoundBuffer(event.data, tick);
			_lastSampleDataPosition = event.position;
		}
	}
}