import Link from "next/link";

export default function Home() {
    return (
        <div>
            <h1>Welcome to the CompWolf website.</h1>
            <p>To learn how to use the CompWolf libraries, consider going through <Link href="/tutorial">these tutorials</Link>.</p>
            <p>If you wish to find specific information about CompWolf, consider checking out <Link href="/api/">its api documentation.</Link>.</p>
            <p>If you wish to see CompWolf's source code, check out its <Link href="https://github.com/Pink-Wolf/BachelorProject">GitHub repository</Link>.</p>
        </div>
    );
}
