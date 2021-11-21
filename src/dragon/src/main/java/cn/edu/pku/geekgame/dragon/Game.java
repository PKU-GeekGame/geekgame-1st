package cn.edu.pku.geekgame.dragon;

import org.json.JSONArray;
import org.json.JSONObject;

import java.security.SecureRandom;
import java.time.Duration;
import java.time.Instant;
import java.util.*;

/**
 * Game instance per user
 */
public final class Game {
    private final Map<String, Integer> balls;
    private final Set<Location> airs;

    private long baseSeed;
    private boolean isSuper;

    private final TokenUser currentTokenUser;
    private Instant expiration;

    public static Game refresh(TokenUser user, Game old) {
        if (old == null) {
            return new Game(user);
        }
        if (old.expiration.isBefore(Instant.now())) {
            return new Game(user);
        }
        return old;
    }

    public Game(TokenUser currentTokenUser) {
        this.baseSeed = BASE_SEED_RNG.nextLong();
        this.expiration = Instant.now().plus(EXPIRATION);

        this.isSuper = false;

        this.balls = new LinkedHashMap<>();

        this.airs = new LinkedHashSet<>();
        this.airs.add(new Location(this, 15, 15));
        this.airs.add(new Location(this, 15, 16));
        this.airs.add(new Location(this, 16, 15));
        this.airs.add(new Location(this, 16, 16));

        this.currentTokenUser = currentTokenUser;
    }

    public synchronized JSONObject processBreak(Map<String, List<String>> params) {
        var x = Integer.parseInt(params.get("x").get(0));
        var y = Integer.parseInt(params.get("y").get(0));
        var location = new Location(this, x, y);
        var ball = location.getBall();
        var result = new JSONObject();
        if (ball.isPresent()) {
            this.airs.add(location);
            this.balls.compute(ball.get().name(), (key, value) -> value == null ? 1 : value + 1);
        }
        result.put("balls", this.balls);
        return result;
    }

    public synchronized JSONObject processReset(Map<String, List<String>> params) {
        this.baseSeed = BASE_SEED_RNG.nextLong();
        this.expiration = Instant.now().plus(EXPIRATION);

        this.isSuper = Boolean.parseBoolean(params.get("super").get(0));

        this.balls.clear();

        this.airs.clear();
        this.airs.add(new Location(this, 15, 15));
        this.airs.add(new Location(this, 15, 16));
        this.airs.add(new Location(this, 16, 15));
        this.airs.add(new Location(this, 16, 16));

        return new JSONObject().put("user", this.currentTokenUser);
    }

    public synchronized JSONObject processState(Map<String, List<String>> params) {
        var x = Integer.parseInt(params.get("x").get(0));
        var y = Integer.parseInt(params.get("y").get(0));
        var minX = Math.floorDiv(x, 32) * 32;
        var minY = Math.floorDiv(y, 32) * 32;
        var materials = new JSONArray();
        for (var i = 0; i < 32; ++i) {
            var materialsPerLine = new JSONArray();
            for (var j = 0; j < 32; ++j) {
                var ball = new Location(this, minX + i, minY + j).getBall();
                materialsPerLine.put(ball.isPresent() ? ball.get().name() : "AIR");
            }
            materials.put(materialsPerLine);
        }
        var min = new JSONArray().put(minX).put(minY);
        var result = new JSONObject().put("materials", materials).put("min", min).put("expiration", this.expiration);
        if (this.balls.size() >= 7) {
            result.put("flag", this.isSuper ? this.currentTokenUser.getFlagSuper() : this.currentTokenUser.getFlag());
        }
        return result;
    }

    private static final Duration EXPIRATION = Duration.ofMinutes(20);
    private static final Random BASE_SEED_RNG = new SecureRandom();

    public record Location(Game game, int x, int y) {
        private static final List<Ball> BALL_LIST = List.of(
                new Ball("FIRST", 24, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
                new Ball("SECOND", 48, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
                new Ball("THIRD", 192, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
                new Ball("FOURTH", 1536, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
                new Ball("FIFTH", 24576, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
                new Ball("SIXTH", 393216, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939),
                new Ball("SEVENTH", 25165824, 0x746b, 0x48e5, 0x7d3bfc6f, 0x3f224939));

        private static final List<Ball> BALL_LIST_SUPER = List.of(
                new Ball("FIRST", 29, 0x2e01, 0x76b5, 0x15beccc9, 0x7296ea13),
                new Ball("SECOND", 53, 0x7c31, 0x7edf, 0x289223e1, 0x4878857f),
                new Ball("THIRD", 193, 0x30b9, 0x5fcf, 0x3857dc53, 0x6e3bd3dd),
                new Ball("FOURTH", 1543, 0x1159, 0x1445, 0x18b0a50b, 0x27d58de9),
                new Ball("FIFTH", 24593, 0x683f, 0x42b5, 0x1b7a1d5b, 0x33f4f1a7),
                new Ball("SIXTH", 393241, 0x1d75, 0x623b, 0x70517261, 0x3ac463fb),
                new Ball("SEVENTH", 25165843, 0x4799, 0x3b11, 0x5b07e3b5, 0x6b85fa33));

        private Optional<Ball> getBall() {
            if (!this.game.airs.contains(this)) {
                var ballList = this.game.isSuper ? BALL_LIST_SUPER : BALL_LIST;
                return ballList.stream().filter(ball -> ball.exists(this.game.baseSeed, this.x, this.y)).findFirst();
            }
            return Optional.empty();
        }
    }

    public record Ball(String name, int chunkSize, int a, int b, int c, int d) {
        private boolean exists(long baseSeed, int offsetX, int offsetY) {
            var blockOffsetX = Math.floorMod(offsetX, this.chunkSize);
            var blockOffsetY = Math.floorMod(offsetY, this.chunkSize);
            var chunkOffsetX = (long) Math.floorDiv(offsetX, this.chunkSize);
            var chunkOffsetY = (long) Math.floorDiv(offsetY, this.chunkSize);
            var rng = new Random(this.a * chunkOffsetX + this.b * chunkOffsetY + this.c * baseSeed + this.d);
            return rng.nextInt(this.chunkSize) == blockOffsetX && rng.nextInt(this.chunkSize) == blockOffsetY;
        }
    }
}
