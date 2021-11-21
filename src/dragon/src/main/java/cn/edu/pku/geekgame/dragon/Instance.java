package cn.edu.pku.geekgame.dragon;

import fi.iki.elonen.NanoHTTPD;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.BiFunction;

/**
 * Main class for http server
 */
public final class Instance extends NanoHTTPD {

    public static void main(String[] args) throws Exception {
        var port = System.getenv().getOrDefault("GEEK_DRAGON_PORT", "8080");
        new Instance(Integer.parseInt(port)).start(SOCKET_READ_TIMEOUT, false);
        System.out.println("Geek dragon server now started at localhost:" + port);
        Thread.currentThread().join(); // just make itself deadlocked for running forever
    }

    @Override
    public Response serve(IHTTPSession session) {
        return switch (session.getMethod()) {
            case POST -> switch (session.getUri()) {
                case "/api/state" -> this.api(session, Game::processState);
                case "/api/reset" -> this.api(session, Game::processReset);
                case "/api/break" -> this.api(session, Game::processBreak);
                default -> this.error();
            };
            case GET -> switch (session.getUri()) {
                case "/", "/index.html" -> this.file("/assets/geek_dragon/index.html", MIME_HTML);
                case "/textures/background.png" -> this.file("/assets/geek_dragon/textures/background.png", MIME_PNG);
                case "/textures/player.png" -> this.file("/assets/geek_dragon/textures/player.png", MIME_PNG);
                case "/textures/waiting.png" -> this.file("/assets/geek_dragon/textures/waiting.png", MIME_PNG);
                case "/textures/loading.png" -> this.file("/assets/geek_dragon/textures/loading.png", MIME_PNG);
                case "/textures/balls.png" -> this.file("/assets/geek_dragon/textures/balls.png", MIME_PNG);
                case "/textures/start.png" -> this.file("/assets/geek_dragon/textures/start.png", MIME_PNG);
                case "/favicon.ico" -> this.file("/assets/geek_dragon/favicon.ico", MIME_ICO);
                case "/astar.min.js" -> this.file("/assets/geek_dragon/astar.min.js", MIME_JS);
                case "/index.js" -> this.file("/assets/geek_dragon/index.js", MIME_JS);
                case "/source.zip" -> this.source(System.getenv("GEEK_DRAGON_SOURCE"));
                case "/api/state" -> this.api(session, Game::processState);
                default -> this.error();
            };
            default -> this.error();
        };
    }

    private static final String ERROR_RESPONSE = "SERVER INTERNAL ERROR: Not Found";

    private static final String MIME_JS = "application/javascript";
    private static final String MIME_JSON = "application/json";
    private static final String MIME_ZIP = "application/zip";
    private static final String MIME_ICO = "image/x-icon";
    private static final String MIME_PNG = "image/png";

    private final Map<TokenUser, Game> games;

    private Instance(int port) {
        super(port);
        this.games = new ConcurrentHashMap<>();
    }

    private Response api(IHTTPSession session, BiFunction<Game, Map<String, List<String>>, ?> handler) {
        try {
            var parameters = session.getParameters();
            var user = TokenUser.from(parameters.get("token").get(0));
            //var game = this.games.computeIfAbsent(user, Game::new);
            var game = this.games.compute(user, Game::refresh);
            var response = handler.apply(game, parameters).toString();
            return newFixedLengthResponse(Response.Status.OK, MIME_JSON, response);
        } catch (Exception e) {
            return this.error();
        }
    }

    private Response error() {
        return newFixedLengthResponse(Response.Status.INTERNAL_ERROR, MIME_PLAINTEXT, ERROR_RESPONSE);
    }

    private Response file(String resource, String mimeType) {
        return newChunkedResponse(Response.Status.OK, mimeType, Instance.class.getResourceAsStream(resource));
    }

    private Response source(String source) {
        try {
            return newChunkedResponse(Response.Status.OK, MIME_ZIP, Files.newInputStream(Path.of(source)));
        } catch (Exception e) {
            return this.error();
        }
    }
}
