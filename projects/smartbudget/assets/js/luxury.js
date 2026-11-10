function initParticles(canvasId) {
    const canvas = document.getElementById(canvasId);
    if (!canvas) return;
    const ctx = canvas.getContext('2d');

    function resize() {
        canvas.width  = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    resize();
    window.addEventListener('resize', resize);

    const GOLD = 'rgba(201,168,76,';
    const particles = [];
    const COUNT = 90;
    let mouse = { x: window.innerWidth / 2, y: window.innerHeight / 2 };

    window.addEventListener('mousemove', e => { mouse.x = e.clientX; mouse.y = e.clientY; });

    for (let i = 0; i < COUNT; i++) {
        particles.push({
            x: Math.random() * window.innerWidth,
            y: Math.random() * window.innerHeight,
            r: Math.random() * 1.4 + .3,
            vx: (Math.random() - .5) * .35,
            vy: (Math.random() - .5) * .35,
            alpha: Math.random() * .5 + .1,
        });
    }

    function draw() {
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        for (let i = 0; i < particles.length; i++) {
            for (let j = i + 1; j < particles.length; j++) {
                const dx = particles[i].x - particles[j].x;
                const dy = particles[i].y - particles[j].y;
                const dist = Math.sqrt(dx * dx + dy * dy);
                if (dist < 130) {
                    ctx.beginPath();
                    ctx.strokeStyle = GOLD + ((1 - dist / 130) * .12) + ')';
                    ctx.lineWidth = .5;
                    ctx.moveTo(particles[i].x, particles[i].y);
                    ctx.lineTo(particles[j].x, particles[j].y);
                    ctx.stroke();
                }
            }
        }

        particles.forEach(p => {
            const dx = p.x - mouse.x;
            const dy = p.y - mouse.y;
            const dist = Math.sqrt(dx * dx + dy * dy);
            if (dist < 180) {
                ctx.beginPath();
                ctx.strokeStyle = GOLD + ((1 - dist / 180) * .25) + ')';
                ctx.lineWidth = .8;
                ctx.moveTo(p.x, p.y);
                ctx.lineTo(mouse.x, mouse.y);
                ctx.stroke();
            }
        });

        particles.forEach(p => {
            ctx.beginPath();
            ctx.arc(p.x, p.y, p.r, 0, Math.PI * 2);
            ctx.fillStyle = GOLD + p.alpha + ')';
            ctx.fill();

            p.x += p.vx;
            p.y += p.vy;

            if (p.x < 0 || p.x > canvas.width)  p.vx *= -1;
            if (p.y < 0 || p.y > canvas.height) p.vy *= -1;
        });

        requestAnimationFrame(draw);
    }
    draw();
}

function init3DTilt(selector, intensity) {
    intensity = intensity || 8;
    document.querySelectorAll(selector).forEach(card => {
        card.style.transition = 'transform .15s ease, box-shadow .15s ease';
        card.style.willChange = 'transform';

        card.addEventListener('mousemove', e => {
            const r  = card.getBoundingClientRect();
            const x  = (e.clientX - r.left) / r.width  - .5;
            const y  = (e.clientY - r.top)  / r.height - .5;
            card.style.transform = `perspective(700px) rotateY(${x * intensity}deg) rotateX(${-y * intensity}deg) scale3d(1.015,1.015,1.015)`;
            card.style.boxShadow = `${-x * 20}px ${-y * 20}px 40px rgba(201,168,76,.12), 0 0 60px rgba(201,168,76,.05)`;
        });

        card.addEventListener('mouseleave', () => {
            card.style.transform = 'perspective(700px) rotateY(0) rotateX(0) scale3d(1,1,1)';
            card.style.boxShadow = 'none';
        });
    });
}

function animateCount(el) {
    if (!el) return;
    const text   = el.textContent.trim();
    const match  = text.match(/([\d\s,.]+)/);
    if (!match) return;

    // PHP number_format: "5,000.00" — strip thousands commas before parsing
    const raw    = match[1].replace(/\s/g, '').replace(/,(?=\d{3})/g, '');
    const target = parseFloat(raw);
    if (isNaN(target) || target === 0) return;

    const isSEK  = text.includes('kr');
    const isPct  = text.includes('%');
    const dur    = 1400;
    const start  = performance.now();

    function step(now) {
        const t       = Math.min((now - start) / dur, 1);
        const ease    = 1 - Math.pow(1 - t, 3);
        const current = target * ease;
        const fmt     = current.toLocaleString('sv-SE', { minimumFractionDigits: isSEK ? 2 : 0, maximumFractionDigits: isSEK ? 2 : 0 });
        el.textContent = fmt + (isSEK ? ' kr' : '') + (isPct ? '%' : '');
        if (t < 1) requestAnimationFrame(step);
    }
    requestAnimationFrame(step);
}

function initScrollReveal() {
    const els = document.querySelectorAll('.section-block, .economy-summary, .stats-card, .card, h1');
    if (!els.length) return;

    const obs = new IntersectionObserver((entries) => {
        entries.forEach(e => {
            if (e.isIntersecting) {
                e.target.style.animation = 'fadeUp .7s ease both';
                obs.unobserve(e.target);
            }
        });
    }, { threshold: .12 });

    els.forEach(el => {
        el.style.opacity = '0';
        el.style.animation = 'none';
        obs.observe(el);
    });
}

function initMagneticButtons() {
    document.querySelectorAll('.primary-btn, .secondary-btn, .save-btn, .add-btn, .auth-card button').forEach(btn => {
        btn.addEventListener('mousemove', e => {
            const r  = btn.getBoundingClientRect();
            const x  = (e.clientX - r.left - r.width  / 2) * .25;
            const y  = (e.clientY - r.top  - r.height / 2) * .25;
            btn.style.transform = `translate(${x}px, ${y}px)`;
        });
        btn.addEventListener('mouseleave', () => {
            btn.style.transform = 'translate(0,0)';
        });
    });
}

document.addEventListener('DOMContentLoaded', () => {
    initParticles('luxury-canvas');
    init3DTilt('.economy-summary, .stats-card, .card, .auth-card', 6);
    initScrollReveal();
    initMagneticButtons();
    document.querySelectorAll('.big-number, .meta-val').forEach(animateCount);
});
