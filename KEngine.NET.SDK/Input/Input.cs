using System.Runtime.CompilerServices;

namespace KEngine.Input
{
    public static class Input
    {

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyDown(string key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyUp(string key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKey(string key);
    }
}
