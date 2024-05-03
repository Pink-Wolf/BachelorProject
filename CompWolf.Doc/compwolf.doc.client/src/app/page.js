import Link from "next/link";

export default function Home() {
    return (
        <div>
            Welcome to the CompWolf docs.
            <ul>
                <li><Link href="/api/Graphics.Core">API docs</Link></li>
                <li><Link href="/tutorial">Tutorials</Link></li>
            </ul>
            
        </div>
    );
}
