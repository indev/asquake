package se.quake
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;

	public class VideoBuffer extends Bitmap
	{
		public static var QUAKE_WIDTH:uint = 320;
		public static var QUAKE_HEIGHT:uint = 240; 
		
		private var _bitmapData:BitmapData;
		private var _rect:Rectangle;
		
		private var _RAM:ByteArray; 	// Quake VM RAM
		
		private var _palette:Array;
		
		private var _pixelBuffer:ByteArray;
		
		public function VideoBuffer()
		{
			var ram_NS:Namespace = new Namespace("cmodule.swc.quake");
			_RAM = (ram_NS::gstate).ds;
			
			_bitmapData = new BitmapData( QUAKE_WIDTH, QUAKE_HEIGHT, false );
			
			_rect = new Rectangle(0,0,QUAKE_WIDTH,QUAKE_HEIGHT);
			
			_pixelBuffer = new ByteArray();
			
			super(_bitmapData, "auto", false);
		}
		
		public function setPalette( ptrAddr:uint ):void
		{
			_palette = [];
			
			_RAM.position = ptrAddr;
			
			var i:int = 0;
			for ( ; i < 256; ++i )
			{
				_palette.push( _RAM.readUnsignedInt() );
			}
		}
		
		public function updateVideoBufferPtr( ptrAddr:uint ):void
		{
			_RAM.position = ptrAddr;
			
			var w:int;
			var h:int;
			var byte:int;
			
			_pixelBuffer.clear();
			
			for ( ; w < QUAKE_WIDTH; ++w ) {
				for ( h = 0; h < QUAKE_HEIGHT; ++h ) {
					_pixelBuffer.writeUnsignedInt( _palette[ _RAM.readUnsignedByte() ] );
				}
			}
			_pixelBuffer.position = 0;
			
			_bitmapData.setPixels( _rect, _pixelBuffer );
		}
		
	}
}