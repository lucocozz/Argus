import Link from '@docusaurus/Link';

export default function Hero() {
  return (
    <header className="hero" style={{
      background: 'radial-gradient(ellipse at center, rgba(76, 175, 80, 0.03) 0%, transparent 70%)',
      paddingBottom: '3rem'
    }}>
      <div className="container">
        <div style={{ textAlign: 'center', maxWidth: '800px', margin: '0 auto' }}>
          <img 
            src="/img/argus-logo.webp" 
            alt="Argus" 
            style={{
              height: '256px',
              width: 'auto',
              marginBottom: '2rem',
              marginTop: '-2rem',
              opacity: 1,
            }} 
          />
          
          <h1 className="hero__title" style={{ marginBottom: '1.5rem' }}>
            Modern CLI Parsing for C
          </h1>

          <p className="hero__subtitle" style={{ marginBottom: '2.5rem' }}>
            Modern C library for command-line argument parsing with a powerful, declarative API
          </p>
          
          <div style={{ 
            display: 'flex', 
            gap: '1rem', 
            justifyContent: 'center', 
            flexWrap: 'wrap',
            marginBottom: '4rem'
          }}>
            <Link
              className="button button--primary button--lg"
              to="/getting-started/quickstart">
              Get Started
            </Link>
            <Link
              className="button button--secondary button--lg"
              to="/api-reference/overview">
              API Reference
            </Link>
          </div>
        </div>
      </div>

      {/* CSS Animation */}
      <style jsx>{`
        @keyframes pulse {
          0%, 100% { 
            transform: scale(1); 
            opacity: 1; 
          }
          50% { 
            transform: scale(1.02); 
            opacity: 0.9; 
          }
        }
      `}</style>
    </header>
  );
}
