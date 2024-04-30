import Link from "next/link";

export default function Home() {
    return (
        <div>
            <h1>Tutorials</h1>
            <sl>
                <li><Link href="/tutorial/your-first-window">Your First Window</Link></li>
                <li><Link href="/tutorial/your-first-triangle">Your First Triangle</Link></li>
            </sl>
        </div>
    );
}
