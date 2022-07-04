package arch.sm213.machine.student;

import machine.AbstractMainMemory;
import org.junit.jupiter.api.Test;

import static org.junit.Assert.*;

public class MainMemoryTest {

    @Test
    public void isAlignedTest() {
        MainMemory mainMemory = new MainMemory(4);
        assertTrue(mainMemory.isAccessAligned(0,1));
        assertTrue(mainMemory.isAccessAligned(0,4));
        assertTrue(mainMemory.isAccessAligned(4,2));
        assertFalse(mainMemory.isAccessAligned(1,2));
        assertFalse(mainMemory.isAccessAligned(3,4));
    }

    @Test
    public void bytesToIntegerTest() {
        MainMemory mainMemory = new MainMemory(4);
        assertEquals(0,mainMemory.bytesToInteger((byte) 0x00,(byte) 0x00,(byte) 0x00,(byte) 0x00));
        assertEquals(1,mainMemory.bytesToInteger((byte) 0x00,(byte) 0x00,(byte) 0x00,(byte) 0x01));
        assertEquals(2147483647,mainMemory.bytesToInteger((byte) 0x7f,(byte) 0xff,(byte) 0xff,(byte) 0xff));
        assertEquals(-2147483648,mainMemory.bytesToInteger((byte) 0x80,(byte) 0x00,(byte) 0x00,(byte) 0x00));
        assertEquals(-1,mainMemory.bytesToInteger((byte) 0xff,(byte) 0xff,(byte) 0xff,(byte) 0xff));
    }

    @Test
    public void integerToBytesTest() {
        MainMemory mainMemory = new MainMemory(4);
        byteListCompare(new byte[]{(byte) 0x00, (byte)0x00,(byte)0x00,(byte)0x00}, mainMemory.integerToBytes(0));
        byteListCompare(new byte[]{(byte) 0x00, (byte)0x00,(byte)0x00,(byte)0x01}, mainMemory.integerToBytes(1));
        byteListCompare(new byte[]{(byte) 0x7f, (byte)0xff,(byte)0xff,(byte)0xff},
                mainMemory.integerToBytes(2147483647));
        byteListCompare(new byte[]{(byte) 0x80, (byte)0x00,(byte)0x00,(byte)0x00},
                mainMemory.integerToBytes(-2147483648));
        byteListCompare(new byte[]{(byte) 0xff, (byte)0xff,(byte)0xff,(byte)0xff}, mainMemory.integerToBytes(-1));
    }

    @Test
    public void setGetTest() {
        MainMemory mainMemory = new MainMemory(4);
        try {
            mainMemory.set(-1, mainMemory.integerToBytes(0));
            fail("not catch the exception");
        } catch (AbstractMainMemory.InvalidAddressException e) {
            // pass
        }

        try {
            mainMemory.set(3, new byte[]{(byte)0x00,(byte)0x00});
            fail("not catch the exception");
        } catch (AbstractMainMemory.InvalidAddressException e) {
            // pass
        }

        try {
            mainMemory.set(0,new byte[]{(byte) 0x00,(byte) 0x11,(byte) 0x22, (byte) 0x33});
            byteListCompare(new byte[]{(byte) 0x00,(byte) 0x11,(byte) 0x22, (byte) 0x33},
                    mainMemory.get(0,4));
            byteListCompare(new byte[]{(byte) 0x11,(byte) 0x22} ,
                    mainMemory.get(1,2));

            mainMemory.set(2,new byte[]{(byte) 0xff,(byte) 0xdd});
            byteListCompare(new byte[]{(byte) 0xff} , mainMemory.get(2,1));
        } catch (AbstractMainMemory.InvalidAddressException e) {
            fail();
        }

        try {
            mainMemory.set(0,new byte[]{(byte) 0x00,(byte) 0x11,(byte) 0x22, (byte) 0x33});
            byte[] result = mainMemory.get(-1,4);
            fail("not catching the exception");
        } catch (AbstractMainMemory.InvalidAddressException e) {
            // pass
        }

        try {
            mainMemory.set(0,new byte[]{(byte) 0x00,(byte) 0x11,(byte) 0x22, (byte) 0x33});
            byte[] result = mainMemory.get(1,4);
            fail("not catching the exception");
        } catch (AbstractMainMemory.InvalidAddressException e) {
            // pass
        }

    }

    public void byteListCompare(byte[] a, byte[] b){
        assertTrue(a.length == b.length);
        for(int i = 0; i < a.length; i++) {
            assertEquals(a[i], b[i]);
        }
    }
}
