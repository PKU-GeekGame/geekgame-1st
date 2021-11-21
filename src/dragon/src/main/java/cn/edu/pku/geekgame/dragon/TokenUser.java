package cn.edu.pku.geekgame.dragon;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.security.*;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.util.Base64;

/**
 * Internal class for token verification
 */
public record TokenUser(String id, long tokenHash, long tokenHashSuper) {
    private static final Certificate CERTIFICATE;

    static {
        Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
        try (var stream = TokenUser.class.getResourceAsStream("/assets/geek_dragon/cert.pem")) {
            CERTIFICATE = CertificateFactory.getInstance("X.509", "BC").generateCertificate(stream);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public static TokenUser from(String token) throws GeneralSecurityException {
        var id = check(token);
        var input = (System.getenv("GEEK_DRAGON_SECRET") + ":" + token).getBytes(StandardCharsets.UTF_8);
        var inputTokenDigest = ByteBuffer.wrap(MessageDigest.getInstance("SHA-256", "BC").digest(input)).getLong();
        var inputSuper = (System.getenv("GEEK_DRAGON_SECRET_SUPER") + ":" + token).getBytes(StandardCharsets.UTF_8);
        var inputTokenDigestSuper = ByteBuffer.wrap(MessageDigest.getInstance("SHA-256", "BC").digest(inputSuper)).getLong();
        return new TokenUser(id, inputTokenDigest, inputTokenDigestSuper);
    }

    private static String check(String tokenString) throws GeneralSecurityException {
        var user = tokenString.substring(0, tokenString.indexOf(':'));
        var sig = tokenString.substring(tokenString.indexOf(':') + 1);
        var userBytes = user.getBytes(StandardCharsets.UTF_8);
        var sigBytes = Base64.getDecoder().decode(sig);
        verify(userBytes, sigBytes);
        return user;
    }

    private static void verify(byte[] signedDataBytes, byte[] sigBytes) throws GeneralSecurityException {
        var signature = Signature.getInstance("SHA256withECDSA", "BC");
        signature.initVerify(CERTIFICATE);
        signature.update(signedDataBytes);
        if (!signature.verify(sigBytes)) {
            throw new SignatureException("invalid signature");
        }
    }

    public String getFlag() {
        return String.format("flag{%s_%016x}", System.getenv("GEEK_DRAGON_PREFIX"), this.tokenHash);
    }

    public String getFlagSuper() {
        return String.format("flag{%s_%016x}", System.getenv("GEEK_DRAGON_PREFIX_SUPER"), this.tokenHashSuper);
    }

    @Override
    public String toString() {
        return this.id;
    }
}
