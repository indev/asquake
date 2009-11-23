package {
	import flash.display.Sprite;
	
	import se.quake.FlashQuake;

	[SWF(width="320", height="240", frameRate="30", backgroundColor="#000000")]
	public class quake extends Sprite
	{
		private var flashQuake:FlashQuake
		public function quake()
		{
			flashQuake = new FlashQuake();
			addChild(flashQuake);
		}
	}
}
