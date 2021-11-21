'use strict'

window.addEventListener('DOMContentLoaded', async () => {
  const loadingProgressHandler = {now: 0, total: 6, callback: () => null}
  const loadingStateHandler = {lastTime: Date.now(), lastMouseMove: null, lastClick: null}

  const loadingCtx = createCanvasContext(loadingStateHandler, [renderLoadingCanvas, processLoadingStates])

  /* RESOURCE LOADING STARTED */

  const materials = [], defaultPos = [15, 15]

  window.materials = materials

  const renderHandler = {center: [16, 16], pos: defaultPos}
  const damageHandler = {lastTime: Date.now(), target: null}
  const areaHandler = {min: defaultPos, max: defaultPos, pending: new Set()}
  const playerHandler = {pos: defaultPos, balls: {}, super: false, flag: null}
  const pathHandler = {lastTime: Date.now(), targets: [], callback: () => null}
  const stateHandler = {lastTime: Date.now(), lastMouseMove: null, lastClick: null}

  let token = (new URLSearchParams(location.search).get('token') || localStorage.token).replaceAll(' ','+')

  const ballNames = ['FIRST', 'SECOND', 'THIRD', 'FOURTH', 'FIFTH', "SIXTH", 'SEVENTH']

  const backgroundTexture = await loadTexture('background.png')
  const waitingTextures = await loadTexture('waiting.png')
  const loadingTextures = await loadTexture('loading.png')
  const playerTexture = await loadTexture('player.png')
  const ballTextures = await loadTexture('balls.png')
  const startTexture = await loadTexture('start.png')

  var deadline=1e14;

  /* RESOURCE LOADING COMPLETE */

  while (true) {
    try {
      localStorage.token = token
      history.replaceState({}, '', '/')
      break
    } catch (e) {
      if (token) {
        token = window.prompt('Invalid token: ' + token + '\nPlease try another token:', token)
      } else {
        token = window.prompt('Token not detected. \nPlease try another token:')
      }
    }
  }

  await new Promise(next => loadingProgressHandler.callback = next)

  const ctx = createCanvasContext(stateHandler, [renderCanvas, processStates])

  /* RESOURCE LOADING FUNCTIONS */

  async function loadTexture(uri) {
    const image = new Image()
    const data = await new Promise(next => {
      image.onload = () => next(image)
      image.src = 'textures/' + uri
    })
    ++loadingProgressHandler.now
    return data
  }

  async function loadState(x, y) {
    const normalizedX = Math.floor(x / 32) * 32, normalizedY = Math.floor(y / 32) * 32
    const param = new URLSearchParams([['token', token], ['x', normalizedX], ['y', normalizedY]])
    if (!areaHandler.pending.has(param.toString())) {
      areaHandler.pending.add(param.toString())
      try {
        const res = await fetch('api/state?' + param)
        const {materials: localMaterials, min: [minX, minY], expiration: tdeadline,flag} = await res.json()
        const [areaMaxX, areaMaxY] = areaHandler.max, [areaMinX, areaMinY] = areaHandler.min
        deadline=tdeadline;
        for (let i = 0; i < 32; ++i) {
          for (let j = 0; j < 32; ++j) {
            (materials[minX + i] || (materials[minX + i] = []))[minY + j] = localMaterials[i][j]
          }
        }
        areaHandler.max = [Math.max(areaMaxX, minX + 31), Math.max(areaMaxY, minY + 31)]
        areaHandler.min = [Math.min(areaMinX, minX), Math.min(areaMinY, minY)]
        areaHandler.pending.delete(param.toString())
        playerHandler.flag = flag || null
      } catch (e) {
        setTimeout(() => areaHandler.pending.delete(param.toString()), 10000)
      }
    }
  }

  /* CANVAS RENDERING FUNCTIONS */

  function renderLoadingCanvas() {
    const percentage = 1 + 99 * loadingProgressHandler.now / loadingProgressHandler.total
    if (percentage >= 100) {
      loadingCtx.drawImage(startTexture, 115, 200)

      loadingCtx.textAlign = 'center'
      loadingCtx.fillStyle = '#FFFFFFFF'

      loadingCtx.font = 'bold 19px sans-serif'
      loadingCtx.fillText('Basic Mode', 216, 238)
      loadingCtx.fillText('Super Mode', 216, 292)
      loadingCtx.fillText('Download Source', 216, 346)

      loadingCtx.font = '14px sans-serif'
      loadingCtx.fillText('Dragon Ball Simulator', 216, 188)

      loadingCtx.font = 'bold 24px sans-serif'
      loadingCtx.fillText('GEEK DRAGON', 216, 168)
    } else {
      loadingCtx.textAlign = 'center'
      loadingCtx.fillStyle = '#FFFFFFFF'
      loadingCtx.font = 'bold 24px sans-serif'
      loadingCtx.fillText('Loading ... ' + Math.round(Math.sqrt(percentage) * 10) + '%', 216, 288)
    }
  }

  function renderCanvas() {
    renderBalls()
    renderPlayer()
    renderWaiting()
    renderHighlight()
    renderInstruction()
  }

  function renderBalls() {
    ctx.fillStyle = '#999999FF'
    ctx.fillRect(24, 168, 384, 384)
    const [centerX, centerY] = renderHandler.center
    const left = Math.round(centerX) - 18, right = Math.round(centerX) + 18
    const top = Math.round(centerY) - 18, bottom = Math.round(centerY) + 18
    for (let i = left; i < right; ++i) {
      for (let j = top; j < bottom; ++j) {
        const texture = (materials[i] || [])[j]
        if (!texture) {
          loadState(i, j)
          const index = Math.floor(stateHandler.lastTime / 600) % 4
          ctx.drawImage(loadingTextures, 12 * index, 12, 12, 12,
              216 + (i - centerX) * 12, 360 + (j - centerY) * 12, 12, 12)
        } else {
          const index = 1 + ballNames.indexOf(texture)
          ctx.drawImage(ballTextures, 12 * index, 0, 12, 12,
              216 + (i - centerX) * 12, 360 + (j - centerY) * 12, 12, 12)
        }
      }
    }
  }

  function renderPlayer() {
    const [centerX, centerY] = renderHandler.center
    const [interpolatedX, interpolatedY] = renderHandler.pos
    ctx.drawImage(playerTexture, 216 + (interpolatedX - centerX) * 12, 360 + (interpolatedY - centerY) * 12)
  }

  function renderWaiting() {
    if (damageHandler.target) {
      const [damageX, damageY] = damageHandler.target
      const [centerX, centerY] = renderHandler.center
      const waitingIndex = Math.floor((stateHandler.lastTime - damageHandler.lastTime) / 400) % 4
      ctx.drawImage(waitingTextures,
          12 * waitingIndex, 0, 12, 12,
          216 + (damageX - centerX) * 12, 360 + (damageY - centerY) * 12, 12, 12)
    }
  }

  function renderHighlight() {
    if (stateHandler.lastMouseMove) {
      const [offsetX, offsetY] = stateHandler.lastMouseMove
      if (offsetX >= 24 && offsetY >= 168 && offsetX < 408 && offsetY < 552) {
        const [centerX, centerY] = renderHandler.center
        const moveX = Math.floor((offsetX - 216) / 12 + centerX)
        const moveY = Math.floor((offsetY - 360) / 12 + centerY)
        const material = (materials[moveX] || [])[moveY]
        if (material === 'AIR') {
          ctx.fillStyle = '#33FF3333'
          ctx.fillRect(216 + (moveX - centerX) * 12, 360 + (moveY - centerY) * 12, 12, 12)
          return
        }
        if (material) {
          const directions = [[1, 0], [0, -1], [-1, 0], [0, 1]]
          for (const [dx, dy] of directions) {
            const material = (materials[moveX + dx] || [])[moveY + dy]
            if (material === 'AIR') {
              ctx.fillStyle = '#33FF3333'
              ctx.fillRect(216 + (moveX - centerX) * 12, 360 + (moveY - centerY) * 12, 12, 12)
              return
            }
          }
        }
        ctx.fillStyle = '#FF333333'
        ctx.fillRect(216 + (moveX - centerX) * 12, 360 + (moveY - centerY) * 12, 12, 12)
      }
    }
  }

  function renderInstruction() {
    ctx.fillStyle = '#FFFFFFFF'
    ctx.drawImage(backgroundTexture, 0, 0)

    ctx.textAlign = 'center'
    // ctx.font = '14px sans-serif'
    // ctx.fillText('Dragon Ball Simulator', 216, 44)
    console.log(deadline)
    let deat=new Date(deadline)
    let deats=deat.getHours()+":"+deat.getMinutes()+":"+deat.getSeconds()
    ctx.font = '14px sans-serif'
    ctx.fillText('This session will be expire after '+deats, 216, 44)

    ctx.font = 'bold 24px sans-serif'
    ctx.fillText('GEEK DRAGON', 216, 24)

    ctx.font = '9px sans-serif'
    ctx.fillText("COLLECTED", 372, 72)

    ctx.font = 'bold 14px sans-serif'
    ctx.fillText(Object.keys(playerHandler.balls).length + ' / ' + 7, 372, 94)

    ctx.font = '9px sans-serif'
    ctx.fillText("CURRENT MODE", 372, 126)

    ctx.font = 'bold 14px sans-serif'
    ctx.fillText(playerHandler.super ? 'Super' : 'Basic', 372, 148)

    ctx.font = '9px sans-serif'
    ctx.fillText("CURRENT BALLS", 174, 72)

    ctx.textAlign = 'left'
    ctx.font = 'bold 14px sans-serif'
    Object.keys(playerHandler.balls).forEach((item, index) => {
      const column = Math.floor(index / 4), row = index % 4, x = 30 + 138 * column, y = 94 + 18 * row
      ctx.fillText(playerHandler.balls[item] + "×" + item.replace('_', ' '), x, y)
    })
  }

  /* STATE PROCESSING FUNCTIONS */

  function processLoadingStates() {
    if (loadingStateHandler.lastClick) {
      const [offsetX, offsetY] = loadingStateHandler.lastClick
      if (offsetX >= 121 && offsetX < 311 && offsetY >= 206 && offsetY < 254) {
        const closure = async () => {
          const params = new URLSearchParams([['token', token], ['super', false]])
          const res = await fetch('api/reset?' + params, {method: 'POST'})
          playerHandler.super = false
          if (res.ok) {
            await loadState(defaultPos[0], defaultPos[1])
            loadingProgressHandler.callback()
          }
        }
        closure()
      }
      if (offsetX >= 121 && offsetX < 311 && offsetY >= 260 && offsetY < 308) {
        const closure = async () => {
          const params = new URLSearchParams([['token', token], ['super', true]])
          const res = await fetch('api/reset?' + params, {method: 'POST'})
          playerHandler.super = true
          if (res.ok) {
            await loadState(defaultPos[0], defaultPos[1])
            loadingProgressHandler.callback()
          }
        }
        closure()
      }
      if (offsetX >= 121 && offsetX < 311 && offsetY >= 314 && offsetY < 362) {
        const closure = async () => {
          const res = await fetch('source.zip')
          if (res.ok) {
            const url = URL.createObjectURL(await res.blob())
            const a = document.createElement('a')
            document.body.appendChild(a)
            a.download = 'source.zip'
            a.href = url
            a.click()
            document.body.removeChild(a)
            window.URL.revokeObjectURL(url)
            localStorage.downloaded = true
          }
        }
        closure()
      }
      loadingStateHandler.lastClick = null
    }
  }

  function processStates() {
    processTargets()
    processRender()
    processClick()
  }

  function processTargets() {
    while (pathHandler.lastTime + 200 <= stateHandler.lastTime) {
      const targetPos = pathHandler.targets.shift()
      if (targetPos) {
        playerHandler.pos = targetPos
        pathHandler.lastTime += 200
      }
      if (pathHandler.targets.length === 0) {
        pathHandler.lastTime = stateHandler.lastTime
        const callback = pathHandler.callback
        pathHandler.callback = () => null
        callback()
      }
    }
  }

  function processRender() {
    const [x, y] = playerHandler.pos
    const target = pathHandler.targets[0] || playerHandler.pos
    const factor = (stateHandler.lastTime - pathHandler.lastTime) / 200
    const interpolatedX = x * (1 - factor) + target[0] * factor
    const interpolatedY = y * (1 - factor) + target[1] * factor
    renderHandler.pos = [interpolatedX, interpolatedY]
    const newCenterX = Math.min(Math.max(renderHandler.center[0], interpolatedX - 1), interpolatedX + 2)
    const newCenterY = Math.min(Math.max(renderHandler.center[1], interpolatedY - 1), interpolatedY + 2)
    renderHandler.center = [newCenterX, newCenterY]
  }

  function processClick() {
    if (stateHandler.lastClick && !playerHandler.winTime) {
      const [offsetX, offsetY] = stateHandler.lastClick
      if (offsetX >= 24 && offsetY >= 168 && offsetX < 408 && offsetY < 552) {
        const [centerX, centerY] = renderHandler.center
        const clickX = Math.floor((offsetX - 216) / 12 + centerX)
        const clickY = Math.floor((offsetY - 360) / 12 + centerY)
        processMaterialClick(clickX, clickY)
        stateHandler.lastClick = null
      }
    }
  }

  function processMaterialClick(x, y) {
    const material = (materials[x] || [])[y]
    if (material === 'AIR') {
      processMove(x, y)
      return
    }
    if (material) {
      const directions = [[1, 0], [0, 1], [-1, 0], [0, -1]]
      for (const [dx, dy] of directions) {
        const material = (materials[x + dx] || [])[y + dy]
        if (material === 'AIR') {
          processMove(x, y).then(() => processDamage(x, y))
          return
        }
      }
    }
  }

  function processMove(x, y) {
    return new Promise(next => {
      if (pathHandler.targets.length === 0) {
        damageHandler.target = null
        pathHandler.callback = next
        pathHandler.lastTime = stateHandler.lastTime
        pathHandler.targets.push(...processNavigate(playerHandler.pos, [x, y]))
      } else {
        damageHandler.target = null
        pathHandler.callback = next
        pathHandler.targets.splice(1)
        pathHandler.targets.push(...processNavigate(pathHandler.targets[0], [x, y]))
      }
    })
  }

  function processNavigate(source, target) {
    const [areaMaxX, areaMaxY] = areaHandler.max, [areaMinX, areaMinY] = areaHandler.min, input = []
    const [sourceX, sourceY] = source, [targetX, targetY] = target
    for (let i = areaMinX; i <= areaMaxX; ++i) {
      input[i - areaMinX] = []
      for (let j = areaMinY; j <= areaMaxY; ++j) {
        const material = (materials[i] || [])[j]
        input[i - areaMinX][j - areaMinY] = +(material === 'AIR' || i === targetX && j === targetY)
      }
    }
    const graph = new Graph(input)
    const end = graph.grid[targetX - areaMinX][targetY - areaMinY]
    const start = graph.grid[sourceX - areaMinX][sourceY - areaMinY]
    const result = astar.search(graph, start, end)
    return result.map(grid => [grid.x + areaMinX, grid.y + areaMinY]).filter(([x, y]) => materials[x][y] === 'AIR')
  }

  function processDamage(x, y) {
    const params = new URLSearchParams([['token', token], ['x', x], ['y', y]])
    const closure = async () => {
      const response = await (await fetch('api/break?' + params, {method: 'POST'})).json()
      const [damageX, damageY] = damageHandler.target || [NaN, NaN]
      playerHandler.balls = response.balls
      if (x === damageX && y === damageY) {
        (materials[x] = materials[x] || [])[y] = 'AIR'
        damageHandler.target = null
        await processMove(x, y)
      }
    }
    damageHandler.lastTime = stateHandler.lastTime
    damageHandler.target = [x, y]
    closure()
  }

  /* CANVAS CONTEXT CREATING FUNCTION */

  function createCanvasContext(state, callbacks) {
    const canvas = document.createElement('canvas'), ctx = canvas.getContext('2d')

    canvas.style.position = 'absolute'

    let offsetScale = 1

    function loop() {
      state.lastTime = Date.now()

      const scaleHiDPI = devicePixelRatio || 1
      const widthHiDPI = scaleHiDPI * innerWidth, heightHiDPI = scaleHiDPI * innerHeight
      const scale = Math.max(1, Math.floor(Math.min(widthHiDPI / 432, heightHiDPI / 576)))

      canvas.width = scale * 432
      canvas.height = scale * 576
      canvas.style.width = scale * 432 / scaleHiDPI + 'px'
      canvas.style.height = scale * 576 / scaleHiDPI + 'px'
      canvas.style.left = (widthHiDPI / 2 - scale * 216) / scaleHiDPI + 'px'
      canvas.style.top = (heightHiDPI / 2 - scale * 288) / scaleHiDPI + 'px'

      offsetScale = scale / scaleHiDPI

      ctx.scale(scale, scale)

      callbacks.forEach(f => f())

      if (document.body.contains(canvas)) {
        window.requestAnimationFrame(loop)
      }
    }

    window.requestAnimationFrame(loop)

    function transform(e) {
      const x = e.offsetX / offsetScale
      const y = e.offsetY / offsetScale
      return [x, y]
    }

    canvas.addEventListener('mouseenter', e => state.lastMouseMove = transform(e))
    canvas.addEventListener('mousemove', e => state.lastMouseMove = transform(e))
    canvas.addEventListener('mouseleave', () => state.lastMouseMove = null)
    canvas.addEventListener('click', e => state.lastClick = transform(e))

    document.body.appendChild(canvas)

    return ctx
  }
})
