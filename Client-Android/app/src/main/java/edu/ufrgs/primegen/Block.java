package edu.ufrgs.primegen;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by lucas on 6/24/17.
 */

public class Block {

    private String startBlock;
    private String endBlock;

    public Block(String startBlock, String endBlock) {
        this.startBlock = startBlock;
        this.endBlock = endBlock;
    }

    public List<String> generateResult() {
        BigInteger i = new BigInteger(startBlock);
        BigInteger end = new BigInteger(endBlock);

        List<String> primeList = new ArrayList<>(end.subtract(i).intValue());
        if (end.compareTo(BigInteger.valueOf(Long.MAX_VALUE)) < 0) {
            long fi = i.longValue();
            long fend = end.longValue();
            while (fi < fend) {
                if (isPrime(fi)) {
                    primeList.add(String.valueOf(fi));
                }
                fi += 1;
            }
        } else {
            while (i.compareTo(end) <= 0) {
                if (isPrimeBig(i)) {
                    primeList.add(i.toString());
                }
                i = i.add(BigInteger.ONE);
            }
        }

        return primeList;
    }

    private boolean isPrimeBig(BigInteger p) {
            if (p.mod(BigInteger.valueOf(2)).equals(BigInteger.ZERO)) return false;

            BigInteger i = BigInteger.valueOf(3);
            while (i.compareTo(p.divide(BigInteger.valueOf(2))) <= 0) {
                if (p.mod(i).equals(BigInteger.ZERO)) {
                    return false;
                }
                i = i.add(BigInteger.valueOf(2));
            }

            return true;
    }

    private boolean isPrime(long p) {
        if (p % 2 == 0) return false;

        long i = 3;
        while (i < (p / 2)) {
            if (p % i == 0) {
                return false;
            }
            i += 2;
        }

        return true;
    }
}
