import Layout from '@theme/Layout';
import Hero from '@site/src/components/Hero';
import CodeComparison from '@site/src/components/CodeComparison';
import WhyChooseSection from '@site/src/components/WhyChooseSection';
import FinalCTA from '@site/src/components/FinalCTA';

export default function Home() {
  return (
    <Layout description="Modern C library for command-line argument parsing with a powerful, declarative API">
      <Hero />
      <CodeComparison />
      <WhyChooseSection />
      <FinalCTA />
    </Layout>
  );
}
